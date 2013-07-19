
extern "C" {
#include <clang-c/Index.h>
}

#include <string>
#include <iostream>

static std::string to_string(CXString str)
{
	const char * tmp = clang_getCString(str);
	if (tmp) {
		std::string translated = std::string(tmp);
		clang_disposeString(str);
		return translated;
	}
	return "";
}

struct Location
{
	Location(CXCursor cursor)
		: filename("")
		, line(0)
		, column(0)
		, offset(0)
	{
		source_location = clang_getCursorLocation(cursor);
		CXFile file;
		clang_getSpellingLocation(source_location, &file, &line, &column, &offset);
		filename = to_string(clang_getFileName(file));
	}

	CXSourceLocation source_location;
	std::string filename;
	unsigned int line;
	unsigned int column;
	unsigned int offset;
};

static void dump(FILE * stream, CXCursor cursor, unsigned int level)
{
	std::string s = to_string(clang_getCursorSpelling(cursor));
	std::string ks = to_string(clang_getCursorKindSpelling(clang_getCursorKind(cursor)));

	Location loc(cursor);
	fprintf(stream, "%40s : %3u : %20s : %s:%u:%u\n",
		s.c_str(),
		level,
		ks.c_str(),
		loc.filename.c_str(),
		loc.line,
		loc.column);
}

static CXChildVisitResult visitor_autorecursion(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	unsigned int level = *static_cast<unsigned int *>(data);
	dump(stdout, cursor, level);
	return CXChildVisit_Recurse;
}

static std::string collect_namespaces_for(CXCursor cursor)
{
	std::string result = "";

	for (;;) {
		CXCursorKind kind = clang_getCursorKind(cursor);
		if (true
			&& (kind != CXCursor_Namespace)
			&& (kind != CXCursor_ClassDecl)
			&& (kind != CXCursor_StructDecl))
			break;

		result = to_string(clang_getCursorSpelling(cursor)) + "::" + result;
		cursor = clang_getCursorLexicalParent(cursor);
	}

	return result;
}

static CXChildVisitResult collect_base_classes(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	std::string * s = static_cast<std::string *>(data);

	if (clang_getCursorKind(cursor) == CXCursor_CXXBaseSpecifier)
		*s += " " + to_string(clang_getCursorSpelling(cursor)) + " ";

	return CXChildVisit_Continue;
}

static CXChildVisitResult count_method_parameters(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	unsigned int * count = static_cast<unsigned int *>(data);

	if (clang_getCursorKind(cursor) == CXCursor_ParmDecl)
		*count += 1;

	return CXChildVisit_Continue;
}

static CXChildVisitResult visitor_recursive(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	Location loc(cursor);

	// ignore system headers/code
	if (clang_Location_isInSystemHeader(loc.source_location))
		return CXChildVisit_Continue;

	unsigned int * level = static_cast<unsigned int *>(data);
	dump(stdout, cursor, *level);

	std::string s = to_string(clang_getCursorSpelling(cursor));
	std::string usr = to_string(clang_getCursorUSR(cursor));

	if (clang_getCursorKind(cursor) == CXCursor_ClassDecl) {
		std::string base_classes;
		clang_visitChildren(cursor, collect_base_classes, &base_classes);
		fprintf(stderr, "CLASS: %s%s (%s) based on {%s} in %s\n",
			collect_namespaces_for(parent).c_str(),
			s.c_str(),
			usr.c_str(),
			base_classes.c_str(),
			loc.filename.c_str());
	}

	if (clang_getCursorKind(cursor) == CXCursor_CXXMethod
		&& !clang_isCursorDefinition(cursor)) {
		unsigned int num_params = 0;
		clang_visitChildren(cursor, count_method_parameters, &num_params);

		fprintf(stderr, "METHOD: %s%s %u (%s) in %s\n",
			collect_namespaces_for(parent).c_str(),
			s.c_str(),
			num_params,
			usr.c_str(),
			loc.filename.c_str());
	}

	*level += 1;
	clang_visitChildren(cursor, visitor_recursive, data);
	*level -= 1;
	return CXChildVisit_Continue;
}

int main(int argc, char ** argv)
{
	CXIndex index = clang_createIndex(0, 0);
	CXTranslationUnit tu = clang_parseTranslationUnit(
		index, 0, argv, argc, 0, 0, CXTranslationUnit_None);

	unsigned int n = clang_getNumDiagnostics(tu);
	for (unsigned int i = 0; i != n; ++i) {
		CXDiagnostic diag = clang_getDiagnostic(tu, i);
		std::string s = to_string(clang_formatDiagnostic(diag,
			clang_defaultDiagnosticDisplayOptions()));
		printf("%s\n", s.c_str());
	}

	unsigned int level = 0;
	clang_visitChildren(
		clang_getTranslationUnitCursor(tu),
		visitor_recursive,
		&level);

	clang_disposeTranslationUnit(tu);
	clang_disposeIndex(index);
	return 0;
}


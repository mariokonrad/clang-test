
extern "C" {
#include <clang-c/Index.h>
}

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

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

	bool is_in_system_header(void) const;

	friend std::ostream & operator<<(std::ostream &, const Location &);

	CXSourceLocation source_location;
	std::string filename;
	unsigned int line;
	unsigned int column;
	unsigned int offset;
};

bool Location::is_in_system_header(void) const
{
	return clang_Location_isInSystemHeader(source_location);
}

std::ostream & operator<<(std::ostream & os, const Location & loc)
{
	return os
		<< loc.filename
		<< ":"
		<< loc.line
		<< ":"
		<< loc.column
		;
}

static void dump(std::ostream & os, CXCursor cursor, unsigned int level)
{
	using namespace std;

	std::string s = to_string(clang_getCursorSpelling(cursor));
	std::string ks = to_string(clang_getCursorKindSpelling(clang_getCursorKind(cursor)));

	Location loc(cursor);
	os
		<< setw(40) << setfill(' ')
		<< s
		<< " : "
		<< setw(3) << setfill(' ')
		<< level
		<< " : "
		<< setw(30) << setfill(' ')
		<< ks
		<< " : "
		<< loc
		<< std::endl;
}

static CXChildVisitResult visitor_autorecursion(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	unsigned int level = *static_cast<unsigned int *>(data);
	dump(std::cout, cursor, level);
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

static void visit_ClassDecl(CXCursor cursor, CXCursor parent)
{
	std::string base_classes;

	clang_visitChildren(cursor, collect_base_classes, &base_classes);

	std::cerr << "CLASS: "
		<< collect_namespaces_for(parent)
		<< to_string(clang_getCursorSpelling(cursor))
		<< " (" << to_string(clang_getCursorUSR(cursor)) << ")"
		<< " based on {" << base_classes << "}"
		<< " at " << Location(cursor)
		<< std::endl;
}

static void visit_MethodDecl(CXCursor cursor, CXCursor parent)
{
	unsigned int num_params = 0;

	clang_visitChildren(cursor, count_method_parameters, &num_params);

	std::cerr << "METHOD: "
		<< collect_namespaces_for(parent)
		<< to_string(clang_getCursorSpelling(cursor))
		<< " " << num_params
		<< " (" << to_string(clang_getCursorUSR(cursor)) << ")"
		<< " at " << Location(cursor)
		<< std::endl;
}

static void visit_Function(CXCursor cursor, CXCursor parent)
{
	unsigned int num_params = 0;

	clang_visitChildren(cursor, count_method_parameters, &num_params);

	std::cerr << "FUNCTION: "
		<< collect_namespaces_for(parent)
		<< to_string(clang_getCursorSpelling(cursor))
		<< " " << num_params
		<< " (" << to_string(clang_getCursorUSR(cursor)) << ")"
		<< " at " << Location(cursor)
		<< std::endl;
}

static CXChildVisitResult visitor_recursive(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	if (Location(cursor).is_in_system_header())
		return CXChildVisit_Continue;

	unsigned int * level = static_cast<unsigned int *>(data);
	dump(std::cout, cursor, *level);

	switch (clang_getCursorKind(cursor)) {
		case CXCursor_ClassDecl:
			visit_ClassDecl(cursor, parent);
			break;

		case CXCursor_CXXMethod:
std::cerr
	<< "DECL:" << clang_isDeclaration(clang_getCursorKind(cursor))
	<< " DEF:" << clang_isCursorDefinition(cursor)
	<< std::endl;
//			if (!clang_isCursorDefinition(cursor))
				visit_MethodDecl(cursor, parent);
			break;

		case CXCursor_FunctionDecl:
			if (clang_isCursorDefinition(cursor))
				visit_Function(cursor, parent);
			break;

		default:
			break;
	}

	*level += 1;
	clang_visitChildren(cursor, visitor_recursive, data);
	*level -= 1;

	return CXChildVisit_Continue;
}

static void process_file(CXIndex & index, const std::string & filename, int argc, char ** argv)
{
	CXTranslationUnit tu = clang_parseTranslationUnit(
		index, filename.c_str(), argv, argc, 0, 0, CXTranslationUnit_None);

	unsigned int n = clang_getNumDiagnostics(tu);
	for (unsigned int i = 0; i != n; ++i) {
		CXDiagnostic diag = clang_getDiagnostic(tu, i);
		std::cout << to_string(clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions())) << std::endl;
	}

	unsigned int level = 0;
	clang_visitChildren(
		clang_getTranslationUnitCursor(tu),
		visitor_recursive,
		&level);

	clang_disposeTranslationUnit(tu);
}

int main(int argc, char ** argv)
{
	CXIndex index = clang_createIndex(0, 0);

	std::vector<std::string> files;
	for (; argc > 1; --argc, ++argv) {
		if (argv[1] == std::string("--")) {
			++argv;
			--argc;
			break;
		}
		files.push_back(argv[1]);
	}

	++argv;
	--argc;

	for (auto filename : files)
		process_file(index, filename, argc, argv);

	clang_disposeIndex(index);
	return 0;
}


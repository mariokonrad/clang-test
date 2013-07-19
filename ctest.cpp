
extern "C" {
#include <clang-c/Index.h>
}

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

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

static std::string namespaces_to_string(const std::vector<std::string> & namespaces)
{
	std::ostringstream os;

	for (auto ns : namespaces)
		os << ns << "::";

	return os.str();
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
	std::vector<std::string> namespaces;

	for (;;) {
		CXCursorKind kind = clang_getCursorKind(cursor);
		if (true
			&& (kind != CXCursor_Namespace)
			&& (kind != CXCursor_ClassDecl)
			&& (kind != CXCursor_StructDecl))
			break;

		namespaces.push_back(to_string(clang_getCursorSpelling(cursor)));
		cursor = clang_getCursorLexicalParent(cursor);
	}

	std::reverse(namespaces.begin(), namespaces.end());
	return namespaces_to_string(namespaces);
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

static CXChildVisitResult count_base_classes(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	unsigned int * count = static_cast<unsigned int *>(data);

	if (clang_getCursorKind(cursor) == CXCursor_CXXBaseSpecifier)
		*count += 1;

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

static CXChildVisitResult count_class_methods(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	unsigned int * count = static_cast<unsigned int *>(data);

	if (clang_getCursorKind(cursor) == CXCursor_CXXMethod)
		*count += 1;

	return CXChildVisit_Continue;
}

static CXChildVisitResult count_class_fields(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	unsigned int * count = static_cast<unsigned int *>(data);

	if (clang_getCursorKind(cursor) == CXCursor_FieldDecl)
		*count += 1;

	return CXChildVisit_Continue;
}

static CXChildVisitResult count_class_nested_records(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	unsigned int * count = static_cast<unsigned int *>(data);

	switch (clang_getCursorKind(cursor)) {
		case CXCursor_ClassDecl:
		case CXCursor_StructDecl:
		case CXCursor_UnionDecl:
			*count += 1;
			break;

		default:
			break;
	}

	return CXChildVisit_Continue;
}

static void visit_ClassDecl(CXCursor cursor, CXCursor parent)
{
	std::string base_classes;
	unsigned int num_base_classes = 0;
	unsigned int num_methods = 0;
	unsigned int num_fields = 0;

	clang_visitChildren(cursor, collect_base_classes, &base_classes);
	clang_visitChildren(cursor, count_base_classes, &num_base_classes);
	clang_visitChildren(cursor, count_class_methods, &num_methods);
	clang_visitChildren(cursor, count_class_fields, &num_fields);

	std::cerr << "CLASS: "
		<< collect_namespaces_for(parent)
		<< to_string(clang_getCursorSpelling(cursor))
		<< " (" << to_string(clang_getCursorUSR(cursor)) << ")"
		<< " based on (" << num_base_classes << ") {" << base_classes << "}"
		<< " methods:" << num_methods
		<< " fields:" << num_fields
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
		<< " params:" << num_params
		<< " (" << to_string(clang_getCursorUSR(cursor)) << ")"
		<< " at " << Location(cursor)
		<< std::endl;
}

static void visit_Function(CXCursor cursor, CXCursor parent)
{
	if (!clang_isCursorDefinition(cursor))
		return;

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

static void dump_info(CXCursor cursor)
{
	CXCursorKind kind = clang_getCursorKind(cursor);

	std::cerr
		<< "#### {"
		<< " DECL:" << clang_isDeclaration(kind)
		<< " DEF:" << clang_isCursorDefinition(cursor)
		<< " REF:" << clang_isReference(kind)
		<< " STMT:" << clang_isStatement(kind)
		<< " EXPR:" << clang_isExpression(kind)
		<< " ATT:" << clang_isAttribute(kind)
		<< "} : " << to_string(clang_getCursorSpelling(cursor))
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
	// disabled: dump(std::cout, cursor, *level);

	switch (clang_getCursorKind(cursor)) {
		case CXCursor_ClassDecl:
			visit_ClassDecl(cursor, parent);
			break;

		case CXCursor_CXXMethod:
			visit_MethodDecl(cursor, parent);
			break;

		case CXCursor_FunctionDecl:
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


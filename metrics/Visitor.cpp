#include "Visitor.hpp"
#include "string_join.hpp"
#include "Clang.hpp"
#include "Location.hpp"
#include <vector>
#include <algorithm>

Visitor * visitor_cast(CXClientData data)
{
	return static_cast<Visitor *>(data);
}

Visitor::~Visitor()
{}

CXChildVisitResult Visitor::visitor_recursive(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	return visitor_cast(data)->visit(cursor, parent);
}

std::string Visitor::namespace_for(CXCursor cursor) const
{
	std::vector<std::string> namespaces;

	for (;;) {
		cursor = clang_getCursorLexicalParent(cursor);
		CXCursorKind kind = Clang::getCursorKind(cursor);
		if (true
			&& (kind != CXCursor_Namespace)
			&& (kind != CXCursor_ClassDecl)
			&& (kind != CXCursor_StructDecl))
			break;

		namespaces.push_back(Clang::getCursorSpelling(cursor));
	}

	if (namespaces.empty())
		return "";

	std::reverse(namespaces.begin(), namespaces.end());
	return utils::join(namespaces.begin(), namespaces.end(), "::", "", "") + "::";
}

bool Visitor::ignore(CXCursor cursor) const
{
	return Location(cursor).is_in_system_header();
}


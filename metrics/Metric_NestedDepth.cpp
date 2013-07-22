#include "Metric_NestedDepth.hpp"
#include "Clang.hpp"
#include "Location.hpp"
#include "VisitorFactory.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

const VisitorDescriptor Metric_NestedDepth::DESCRIPTOR =
{
	"NstDpt",
	"Nested Depth of Functions and Methods",
	"",
	true,
	false,
	true,
};

void Metric_NestedDepth::register_in(VisitorFactory & factory)
{
	factory.add(DESCRIPTOR, []()
	{
		return new Metric_NestedDepth;
	});
}

const std::string & Metric_NestedDepth::get_name() const
{
	return DESCRIPTOR.name;
}

const std::string & Metric_NestedDepth::get_id() const
{
	return DESCRIPTOR.id;
}

CXChildVisitResult Metric_NestedDepth::visit(
		CXCursor cursor,
		CXCursor parent)
{
	if (Location(cursor).is_in_system_header())
		return CXChildVisit_Continue;

	switch (Clang::getCursorKind(cursor)) {
		case CXCursor_FunctionDecl:
		case CXCursor_CXXMethod:
			if (!Clang::isCursorDefinition(cursor))
				return CXChildVisit_Continue;
			break;

		default:
			return CXChildVisit_Recurse;
	}

	depth = 2;
	depth_max = 1;
	clang_visitChildren(cursor, compute_nested_depth, this);

	std::string usr = Clang::getCursorUSR(cursor);
	if (data.find(usr) != data.end())
		return CXChildVisit_Continue;

	data[usr] =
	{
		cursor,
		depth_max
	};

	return CXChildVisit_Continue;
}

CXChildVisitResult Metric_NestedDepth::compute_nested_depth(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	Metric_NestedDepth * visitor = static_cast<Metric_NestedDepth *>(data);

	unsigned int delta = 0;
	switch (Clang::getCursorKind(cursor)) {
		case CXCursor_CaseStmt:
		case CXCursor_DefaultStmt:
		case CXCursor_ForStmt:
		case CXCursor_CXXForRangeStmt:
		case CXCursor_IfStmt:
			delta = 1;
			if (visitor->depth > visitor->depth_max)
				visitor->depth_max = visitor->depth;
			break;

		default:
			break;
	}

	visitor->depth += delta;
	clang_visitChildren(cursor, compute_nested_depth, data);
	visitor->depth -= delta;

	return CXChildVisit_Continue;
}

void Metric_NestedDepth::report(std::ostream & os) const
{
	using namespace std;

	os << get_name() << endl;

	for (auto i : data) {
		os
			<< setw(3)
			<< i.second.count
			<< " "
			<< namespace_for(i.second.cursor) << Clang::getCursorSpelling(i.second.cursor)
			<< " (" << Location(i.second.cursor) << ")"
			<< endl;
	}
}


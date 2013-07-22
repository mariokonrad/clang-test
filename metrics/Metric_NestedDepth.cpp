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

	// TODO: implementation

	return CXChildVisit_Recurse;
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


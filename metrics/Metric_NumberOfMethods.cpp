#include "Metric_NumberOfMethods.hpp"
#include "Clang.hpp"
#include "Location.hpp"
#include "VisitorFactory.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

const VisitorDescriptor Metric_NumberOfMethods::DESCRIPTOR =
{
	"NumMeth",
	"Number of Methods",
	"",
	true,
};

void Metric_NumberOfMethods::register_in(VisitorFactory & factory)
{
	factory.add(DESCRIPTOR, []()
	{
		return new Metric_NumberOfMethods;
	});
}

const std::string & Metric_NumberOfMethods::get_name() const
{
	return DESCRIPTOR.name;
}

const std::string & Metric_NumberOfMethods::get_id() const
{
	return DESCRIPTOR.id;
}

CXChildVisitResult Metric_NumberOfMethods::count_class_methods(
		CXCursor cursor,
		CXCursor parent,
		CXClientData data)
{
	unsigned int * count = static_cast<unsigned int *>(data);

	if (clang_getCursorKind(cursor) == CXCursor_CXXMethod)
		*count += 1;

	return CXChildVisit_Continue;
}

CXChildVisitResult Metric_NumberOfMethods::visit(
		CXCursor cursor,
		CXCursor parent)
{
	if (Location(cursor).is_in_system_header())
		return CXChildVisit_Continue;
	if (Clang::getCursorKind(cursor) != CXCursor_ClassDecl)
		return CXChildVisit_Recurse;

	std::string usr = Clang::getCursorUSR(cursor);
	if (data.find(usr) != data.end())
		return CXChildVisit_Recurse;

	unsigned int count = 0;
	clang_visitChildren(cursor, count_class_methods, &count);

	data[usr] =
	{
		cursor,
		count
	};

	return CXChildVisit_Recurse;
}

void Metric_NumberOfMethods::report(std::ostream & os) const
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


#include "Metric_FunctionArguments.hpp"
#include "Clang.hpp"
#include "Location.hpp"
#include <iomanip>

const std::string & Metric_FunctionArguments::get_name() const
{
	static const std::string NAME = "Number of Funtion/Method Arguments";
	return NAME;
}

const std::string & Metric_FunctionArguments::get_id() const
{
	static const std::string ID = "NumFuncArg";
	return ID;
}

CXChildVisitResult Metric_FunctionArguments::visit(
		CXCursor cursor,
		CXCursor parent)
{
	if (Location(cursor).is_in_system_header())
		return CXChildVisit_Continue;
	switch (Clang::getCursorKind(cursor)) {
		case CXCursor_CXXMethod:
		case CXCursor_FunctionDecl:
			break;

		default:
			return CXChildVisit_Recurse;
	}

	std::string usr = Clang::getCursorUSR(cursor);
	if (data.find(usr) != data.end())
		return CXChildVisit_Recurse;

	data[usr] =
	{
		cursor,
		Clang::getNumArguments(cursor)
	};

	return CXChildVisit_Recurse;
}

void Metric_FunctionArguments::report(std::ostream & os) const
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


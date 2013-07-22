#include "Metric_FunctionArguments.hpp"
#include "Clang.hpp"
#include "Location.hpp"
#include <iomanip>

const std::string & Metric_FunctionArguments::get_name() const
{
	static const std::string NAME = "Number of Funtion/Method Arguments";
	return NAME;
}

CXChildVisitResult Metric_FunctionArguments::visit(
		CXCursor cursor,
		CXCursor parent)
{
	switch (Clang::getCursorKind(cursor)) {
		case CXCursor_CXXMethod:
		case CXCursor_FunctionDecl:
			break;

		default:
			return CXChildVisit_Recurse;
	}

	std::string usr = Clang::getCursorUSR(cursor);
	if (data.find(usr) != data.end())
		return CXChildVisit_Continue;

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
			<< setw(2)
			<< i.second.count
			<< " "
			<< namespace_for(i.second.cursor) << Clang::getCursorSpelling(i.second.cursor)
			<< " (" << Location(i.second.cursor) << ")"
			<< endl;
	}
}


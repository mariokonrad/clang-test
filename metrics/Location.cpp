#include "Location.hpp"
#include "Clang.hpp"

void Location::init_filename()
{
	CXFile file;
	clang_getSpellingLocation(source_location, &file, &line, &column, &offset);
	filename = Clang::getFileName(file);
}

Location::Location(CXCursor cursor)
	: filename("")
	, line(0)
	, column(0)
	, offset(0)
{
	source_location = Clang::getCursorLocation(cursor);
	init_filename();
}

Location::Location(CXSourceLocation location)
	: source_location(location)
	, filename("")
	, line(0)
	, column(0)
	, offset(0)
{
	init_filename();
}

unsigned int Location::get_line() const
{
	return line;
}

bool Location::is_in_system_header(void) const
{
	return Clang::isInSystemHeader(source_location);
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


#include "Range.hpp"

Range::Range(CXCursor cursor)
	: range(clang_getCursorExtent(cursor))
	, start(clang_getRangeStart(range))
	, end(clang_getRangeEnd(range))
{}

unsigned int Range::get_num_lines() const
{
	return end.get_line() - start.get_line() - 1;
}


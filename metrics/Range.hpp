#ifndef __RANGE__HPP__
#define __RANGE__HPP__

#include "Location.hpp"

class Range
{
	private:
		CXSourceRange range;
		Location start;
		Location end;
	public:
		Range(CXCursor cursor);
		unsigned int get_num_lines() const;
};

#endif

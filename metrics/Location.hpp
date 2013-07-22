#ifndef __LOCATION__HPP__
#define __LOCATION__HPP__

#include <clang-c/Index.h>
#include <string>
#include <ostream>

class Location
{
	private:
		CXSourceLocation source_location;
		std::string filename;
		unsigned int line;
		unsigned int column;
		unsigned int offset;

		void init_filename();
	public:
		Location(CXCursor cursor);
		Location(CXSourceLocation location);
		bool is_in_system_header(void) const;
		unsigned int get_line() const;

		friend std::ostream & operator<<(std::ostream &, const Location &);
};

#endif

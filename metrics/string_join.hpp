#ifndef __STRING_JOIN__HPP__
#define __STRING_JOIN__HPP__

#include <string>
#include <sstream>

namespace utils {

template <class T, class U>
std::ostream & operator<<(
		std::ostream & os,
		const std::pair<T,U> & p)
{
	return os << p.first << "=>" << p.second;
}

template <class T>
std::string join(
		T begin,
		T end,
		const std::string & delm = ",",
		const std::string & prefix = "[",
		const std::string & suffix = "]")
{
	std::ostringstream os;
	os << prefix;
	if (begin != end) {
		os << *begin;
		++begin;
		for (; begin != end; ++begin) {
			os << delm << *begin;
		}
	}
	os << suffix;
	return os.str();
}

}

#endif

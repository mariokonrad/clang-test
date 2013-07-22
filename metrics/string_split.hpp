#ifndef __STRING_SPLIT__H__
#define __STRING_SPLIT__H__

#include <string>
#include <vector>
#include <sstream>

namespace utils {

template <class Container>
Container & split(
		Container & v,
		const std::string & s,
		const std::string & delm = " ")
{
	using namespace std;

	string::size_type last = s.find_first_not_of(delm, 0);
	string::size_type p = s.find_first_of(delm, last);
	for (; p != string::npos || last != string::npos;) {
		typename Container::value_type t;
		std::istringstream is(s.substr(last, p - last));
		if (!(is >> t))
			break;
		v.push_back(t);
		last = s.find_first_not_of(delm, p);
		p = s.find_first_of(delm, last);
	}
	return v;
}

template <class Container>
Container split(
		const std::string & s,
		const std::string & delm = " ")
{
	Container v;
	split(v, s, delm);
	return v;
}

}

#endif

#ifndef __VISITORDESCRIPTOR__HPP__
#define __VISITORDESCRIPTOR__HPP__

#include <string>

class VisitorDescriptor
{
	public:
		std::string id;
		std::string name;
		std::string description;
		bool included_in_all;
		bool reports_records; // provides reports about records (structs, classes)
		bool reports_functions; // provides reports about function (methods)
};

#endif

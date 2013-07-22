#ifndef __METRIC_NUMBEROFFIELDS__HPP__
#define __METRIC_NUMBEROFFIELDS__HPP__

#include "Visitor.hpp"
#include <map>

class Metric_NumberOfFields : public Visitor
{
	private:
		struct Info
		{
			CXCursor cursor;
			unsigned int count;
		};

		typedef std::map<std::string, Info> Data;

		Data data;

	private:
		static CXChildVisitResult count_class_fields(
				CXCursor cursor,
				CXCursor parent,
				CXClientData data);

	public:
		virtual const std::string & get_name() const;

		virtual CXChildVisitResult visit(
				CXCursor cursor,
				CXCursor parent);

		virtual void report(std::ostream &) const;
};

#endif

#ifndef __METRIC_FUNCTIONARGUMENTS__HPP__
#define __METRIC_FUNCTIONARGUMENTS__HPP__

#include "Visitor.hpp"
#include <map>

class Metric_FunctionArguments : public Visitor
{
	private:
		struct Info
		{
			CXCursor cursor;
			int count;
		};

		typedef std::map<std::string, Info> Data;

		Data data;

	public:
		virtual const std::string & get_name() const;
		virtual const std::string & get_id() const;

		virtual CXChildVisitResult visit(
				CXCursor cursor,
				CXCursor parent);

		virtual void report(std::ostream &) const;
};

#endif

#ifndef __METRIC_FUNCTIONARGUMENTS__HPP__
#define __METRIC_FUNCTIONARGUMENTS__HPP__

#include "Visitor.hpp"
#include "VisitorDescriptor.hpp"
#include <map>

class VisitorFactory;

class Metric_FunctionArguments : public Visitor
{
	private:
		struct Item
		{
			CXCursor cursor;
			int count;
		};

		std::map<std::string, Item> data;

		static const VisitorDescriptor DESCRIPTOR;

	public:
		virtual const std::string & get_name() const;
		virtual const std::string & get_id() const;

		virtual CXChildVisitResult visit(
				CXCursor cursor,
				CXCursor parent);

		virtual void report(std::ostream &) const;

		static void register_in(VisitorFactory &);
};

#endif

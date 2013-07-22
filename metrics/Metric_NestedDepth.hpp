#ifndef __METRIC_NestedDepth__HPP__
#define __METRIC_NestedDepth__HPP__

#include "Visitor.hpp"
#include "VisitorDescriptor.hpp"
#include <map>

class VisitorFactory;

class Metric_NestedDepth : public Visitor
{
	private:
		struct Item
		{
			CXCursor cursor;
			unsigned int count;
		};

		std::map<std::string, Item> data;

		static const VisitorDescriptor DESCRIPTOR;

		unsigned int depth;
		unsigned int depth_max;

	private:

		static CXChildVisitResult compute_nested_depth(
				CXCursor cursor,
				CXCursor parent,
				CXClientData data);

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
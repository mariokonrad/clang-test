#ifndef __METRIC_NUMBEROFMETHODS__HPP__
#define __METRIC_NUMBEROFMETHODS__HPP__

#include "Visitor.hpp"
#include "VisitorDescriptor.hpp"
#include <map>

class VisitorFactory;

class Metric_NumberOfMethods : public Visitor
{
	private:
		struct Item
		{
			CXCursor cursor;
			std::string result;
		};

		std::map<std::string, Item> data;

		static const VisitorDescriptor DESCRIPTOR;

	private:
		static CXChildVisitResult count_class_methods(
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
		virtual void collect(ResultContainer &) const;

		static void register_in(VisitorFactory &);
};

#endif

#ifndef __METRIC_DIT__HPP__
#define __METRIC_DIT__HPP__

#include "Visitor.hpp"
#include <map>

class Metric_DIT : public Visitor
{
	private:
		struct Info
		{
			CXCursor cursor;
			unsigned int depth;
		};

		typedef std::map<std::string, Info> Data;

		Data data;

	private:
		unsigned int count_depth_of_inheritance_tree(CXCursor cursor) const;

		static CXChildVisitResult collect_base_classes(
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

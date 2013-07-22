#ifndef __ASTDUMP__HPP__
#define __ASTDUMP__HPP__

#include "Visitor.hpp"
#include "VisitorDescriptor.hpp"
#include <map>

class VisitorFactory;

class ASTDump : public Visitor
{
	private:
		unsigned int level;
		static const VisitorDescriptor DESCRIPTOR;

	public:
		ASTDump();

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

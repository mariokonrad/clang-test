#include "ASTDump.hpp"
#include "Clang.hpp"
#include "Location.hpp"
#include "VisitorFactory.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

const VisitorDescriptor ASTDump::DESCRIPTOR =
{
	"ast-dump",
	"Dumps the AST while traversing the tree to stdout.",
	"",
	false,
};

void ASTDump::register_in(VisitorFactory & factory)
{
	factory.add(DESCRIPTOR, []()
	{
		return new ASTDump;
	});
}

ASTDump::ASTDump()
	: level(0)
{}

const std::string & ASTDump::get_name() const
{
	return DESCRIPTOR.name;
}

const std::string & ASTDump::get_id() const
{
	return DESCRIPTOR.id;
}

CXChildVisitResult ASTDump::visit(
		CXCursor cursor,
		CXCursor parent)
{
	if (Location(cursor).is_in_system_header())
		return CXChildVisit_Continue;

	for (unsigned int i = 0; i < level; ++i)
		std::cout << "    ";
	std::cout
		<< Clang::getCursorKindSpelling(cursor)
		<< "  " << Clang::getCursorSpelling(cursor)
		<< std::endl;

	++level;
	clang_visitChildren(cursor, Visitor::visitor_recursive, this);
	--level;
	return CXChildVisit_Continue;
}

void ASTDump::report(std::ostream & os) const
{}


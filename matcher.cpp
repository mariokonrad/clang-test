#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <iostream>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::extrahelp MoreHelp("\nMore help text...");

StatementMatcher loopMatcher =
	forStmt(
		hasLoopInit(
			declStmt(
				hasSingleDecl(
					varDecl(
						hasInitializer(
							integerLiteral(
								equals(0)))))))).bind("forLoop");

class LoopPrinter : public MatchFinder::MatchCallback
{
	public :
		virtual void run(const MatchFinder::MatchResult & Result)
		{
			if (const ForStmt * s = Result.Nodes.getNodeAs<clang::ForStmt>("forLoop"))
				s->dump();
		}
};

int main(int argc, const char ** argv)
{
	CommonOptionsParser OptionsParser(argc, argv);
	ClangTool tool(
			OptionsParser.getCompilations(),
			OptionsParser.getSourcePathList());

//	return tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>());

	LoopPrinter printer;
	MatchFinder finder;
	finder.addMatcher(loopMatcher, &printer);

	return tool.run(newFrontendActionFactory(&finder));
}


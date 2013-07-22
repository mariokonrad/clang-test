#include "Clang.hpp"
#include "Metric_DIT.hpp"
#include "Metric_FunctionArguments.hpp"
#include <iostream>
#include <string>
#include <vector>

static CXTranslationUnit process_file(
		std::vector<Visitor *> & visitors,
		CXIndex & index,
		const std::string & filename,
		int argc, char ** argv)
{
	CXTranslationUnit tu = Clang::parseTranslationUnit(
		index, filename, argv, argc, 0, 0, CXTranslationUnit_None);

	const unsigned int N = Clang::getNumDiagnostics(tu);
	for (unsigned int i = 0; i != N; ++i) {
		std::cerr
			<< Clang::formatDiagnostic(Clang::getDiagnostic(tu, i))
			<< std::endl;
	}

	if (N == 0) {
		for (auto visitor : visitors)
			clang_visitChildren(
				Clang::getTranslationUnitCursor(tu),
				Visitor::visitor_recursive,
				visitor);
	}

	return tu;
}

int main(int argc, char ** argv)
{
	CXIndex index = clang_createIndex(0, 0);

	std::vector<std::string> files;
	for (; argc > 1; --argc, ++argv) {
		if (argv[1] == std::string("--")) {
			++argv;
			--argc;
			break;
		}
		files.push_back(argv[1]);
	}

	++argv;
	--argc;

	std::vector<Visitor *> visitors;
	visitors.push_back(new Metric_DIT);
	visitors.push_back(new Metric_FunctionArguments);

	std::vector<CXTranslationUnit> translationunits;
	for (auto filename : files)
		translationunits.push_back(process_file(visitors, index, filename, argc, argv));

	for (auto visitor : visitors)
		visitor->report(std::cout);

	for (auto tu : translationunits)
		Clang::disposeTranslationUnit(tu);

	clang_disposeIndex(index);
	return 0;
}


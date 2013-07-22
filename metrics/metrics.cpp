#include "Clang.hpp"
#include "Metric_DIT.hpp"
#include "Metric_FunctionArguments.hpp"
#include "Metric_NumberOfMethods.hpp"
#include "Metric_NumberOfFields.hpp"
#include "string_split.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <cstdlib>

struct Options
{
	std::vector<std::string> value_include;
	std::vector<std::string> value_define;
	std::vector<std::string> value_inputfiles;
	std::string value_visitors;
};

static int parse_options(
		Options & options,
		int argc, char ** argv,
		const std::vector<Visitor *> & available_visitors)
{
	using namespace boost::program_options;

	options_description options_generic("Common Options");
	options_generic.add_options()
		("help",
			"show this help message")
		("version",
			"show version information (not implemented yet)")
		;

	options_description options_preproc("Preprocessor Options");
	options_preproc.add_options()
		("include,I",
			value<std::vector<std::string>>(&options.value_include)->composing(),
			"include path")
		("define,D",
			value<std::vector<std::string>>(&options.value_define)->composing(),
			"define preprocessor symbol")
		;

	options_description options_input("Input Options");
	options_input.add_options()
		("input-file",
			value<std::vector<std::string>>(&options.value_inputfiles)->composing(),
			"file to process")
		;

	options_description options_processing("Processing Options");
	options_processing.add_options()
		("process",
			value<std::string>(&options.value_visitors)->composing(),
			"comma separated selection of visitors, use 'all' to enable all")
		;

	positional_options_description positional_options;
	positional_options.add("input-file", -1);

	options_description cli_options;
	cli_options.add(options_generic);
	cli_options.add(options_preproc);
	cli_options.add(options_input);
	cli_options.add(options_processing);

	variables_map vm;
	try {
		store(command_line_parser(argc, argv)
			.options(cli_options)
			.positional(positional_options)
			.run(), vm);

		if (vm.count("help")) {
			using namespace std;

			cout << "usage: " << argv[0] << " [options] files-to-parse" << endl << endl;
			cout << "Parses metrics from specified files." << endl << endl;
			cout << cli_options << endl;
			cout << "Available Visitors:" << endl;
			for (auto v : available_visitors)
				cout
					<< "  "
					<< left << setw(15)
					<< v->get_id()
					<< " : "
					<< v->get_name()
					<< endl;
			cout << endl;
			return EXIT_FAILURE;
		}

		notify(vm);
	} catch (required_option & e) {
		std::cout << "error: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	} catch (error & e) {
		std::cout << "error: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

static CXTranslationUnit process_file(
		std::vector<Visitor *> & visitors,
		CXIndex & index,
		const std::string & filename,
		const std::vector<std::string> & arguments)
{
	// adapt to libclang interface, to do this for every file is a bit overhead
	// but this way the interface is easier and the overhead does not bother.
	char ** argv = new char* [arguments.size()];
	for (size_t i = 0; i < arguments.size(); ++i) {
		argv[i] = const_cast<char *>(arguments[i].c_str());
	}

	CXTranslationUnit tu = Clang::parseTranslationUnit(
		index, filename, argv, arguments.size(), 0, 0, CXTranslationUnit_None);

	delete [] argv;

	const unsigned int N = Clang::getNumDiagnostics(tu);
	for (unsigned int i = 0; i != N; ++i) {
		std::cerr
			<< Clang::formatDiagnostic(Clang::getDiagnostic(tu, i))
			<< std::endl;
	}

	if (N == 0) {
		for (auto visitor : visitors) {
			clang_visitChildren(
				Clang::getTranslationUnitCursor(tu),
				Visitor::visitor_recursive,
				visitor);
		}
	}

	return tu;
}

int main(int argc, char ** argv)
{
	// available visitors: TODO: register factories

	std::vector<Visitor *> available_visitors =
	{
		new Metric_DIT,
		new Metric_NumberOfMethods,
		new Metric_NumberOfFields,
		new Metric_FunctionArguments,
	};

	// command line options

	Options options;
	if (parse_options(options, argc, argv, available_visitors) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	std::vector<std::string> arguments =
	{
		"-std=c++11", // always C++11
	};
	for (auto i : options.value_include)
		arguments.push_back(std::string("-I") + i);
	for (auto i : options.value_define)
		arguments.push_back(std::string("-D") + i);

	// visitor selection

	std::vector<Visitor *> visitors;
	if (options.value_visitors == "all") {
		visitors = available_visitors;
	} else {
		std::vector<std::string> visitor_ids;
		utils::split(visitor_ids, options.value_visitors, ",");
		for (auto id : visitor_ids) {
			auto v = find_if(
				begin(available_visitors),
				end(available_visitors),
				[id](const Visitor * visitor)
			{
				return id == visitor->get_id();
			});
			if (v != end(available_visitors))
				visitors.push_back(*v);
		}
	}

	// process files

	CXIndex index = clang_createIndex(0, 1);

	std::vector<CXTranslationUnit> translationunits;
	for (auto filename : options.value_inputfiles) {
		CXTranslationUnit tu = process_file(
			visitors, index,
			filename, arguments);
		translationunits.push_back(tu);
	}

	for (auto visitor : visitors)
		visitor->report(std::cout);

	for (auto tu : translationunits)
		Clang::disposeTranslationUnit(tu);

	clang_disposeIndex(index);
	return 0;
}


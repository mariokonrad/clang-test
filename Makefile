.PHONY: all clean

all : ctest tool matcher

ctest : ctest.cpp
#	g++ -fno-rtti -fomit-frame-pointer -O2 -fPIC -fno-exceptions -DNDEBUG
	clang++ $(shell llvm-config --cxxflags) -fno-rtti \
		-o $@ $< \
		-lclang \
		-lclangTooling \
		-lclangFrontendTool \
		-lclangFrontend \
		-lclangDriver \
		-lclangSerialization \
		-lclangCodeGen \
		-lclangParse \
		-lclangSema \
		-lclangStaticAnalyzerFrontend \
		-lclangStaticAnalyzerCheckers \
		-lclangStaticAnalyzerCore \
		-lclangAnalysis \
		-lclangARCMigrate \
		-lclangRewriteFrontend \
		-lclangRewriteCore \
		-lclangEdit \
		-lclangAST \
		-lclangLex \
		-lclangBasic \
		$(shell llvm-config --libs cppbackend) \
		$(shell llvm-config --ldflags)

matcher : matcher.cpp
	clang++ $(shell llvm-config --cxxflags) -fno-rtti -std=c++11 \
		-o $@ $< \
		-lclang \
		-lclangTooling \
		-lclangFrontendTool \
		-lclangFrontend \
		-lclangDriver \
		-lclangSerialization \
		-lclangCodeGen \
		-lclangParse \
		-lclangSema \
		-lclangStaticAnalyzerFrontend \
		-lclangStaticAnalyzerCheckers \
		-lclangStaticAnalyzerCore \
		-lclangAnalysis \
		-lclangARCMigrate \
		-lclangRewriteFrontend \
		-lclangRewriteCore \
		-lclangEdit \
		-lclangAST \
		-lclangASTMatchers \
		-lclangLex \
		-lclangBasic \
		$(shell llvm-config --libs all) \
		$(shell llvm-config --ldflags)

tool : tool.cpp
	clang++ $(shell llvm-config --cxxflags) -fno-rtti -std=c++11 \
		-o $@ $< \
		-lclang \
		-lclangTooling \
		-lclangFrontendTool \
		-lclangFrontend \
		-lclangDriver \
		-lclangSerialization \
		-lclangCodeGen \
		-lclangParse \
		-lclangSema \
		-lclangStaticAnalyzerFrontend \
		-lclangStaticAnalyzerCheckers \
		-lclangStaticAnalyzerCore \
		-lclangAnalysis \
		-lclangARCMigrate \
		-lclangRewriteFrontend \
		-lclangRewriteCore \
		-lclangEdit \
		-lclangAST \
		-lclangASTMatchers \
		-lclangLex \
		-lclangBasic \
		$(shell llvm-config --libs all) \
		$(shell llvm-config --ldflags)

clean :
	rm -f ctest *.o tool matcher


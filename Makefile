.PHONY: all clean

# llvm build (http://clang.llvm.org/get_started.html):
#
# svn co http://llvm.org/svn/llvm-project/llvm/tags/RELEASE_33/final llvm
# cd llvm/tools
# svn co http://llvm.org/svn/llvm-project/cfe/tags/RELEASE_33/final clang
# cd ../..
# cd llvm/projects
# svn co http://llvm.org/svn/llvm-project/compiler-rt/tags/RELEASE_33/final compiler-rt
# cd ../..
# mkdir build
# cd build
# ../llvm/configure --prefix=$HOME/local/llvm-3.3 --enable-optimized --enable-cxx11
# make
# make install
#

CONFIG=$(HOME)/local/llvm-3.3/bin/llvm-config

CXX=$(shell $(CONFIG) --bindir)/clang++
CXXFLAGS=$(shell $(CONFIG) --cxxflags) -std=c++11 -O2
LDFLAGS=$(shell $(CONFIG) --ldflags)

CLANGLIBS=\
	-lclang \
	-lclangTooling \
	-lclangFrontendTool \
	-lclangFrontend \
	-lclangParse \
	-lclangSema \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangAnalysis \
	-lclangARCMigrate \
	-lclangEdit \
	-lclangAST \
	-lclangASTMatchers \
	-lclangLex \
	-lclangDriver \
	-lclangSerialization \
	-lclangRewriteCore \
	-lclangCodeGen \
	-lclangRewriteFrontend \
	-lclangBasic

LIBS=\
	-L$(shell $(CONFIG) --libdir) \
	$(CLANGLIBS) \
	$(shell $(CONFIG) --libs core cppbackend cppbackendinfo bitreader mcparser)

all : ctest tool matcher

metrics : metrics.o
	$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

matcher : matcher.o
	$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

ctest : ctest.o
	$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

tool : tool.o
	$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)

clean :
	rm -f *.o
	rm -f ctest tool matcher

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<


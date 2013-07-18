#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Parse/Parser.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <iostream>
#include <sstream>

// maybe it helps:
//   http://clang.llvm.org/docs/RAVFrontendAction.html
//   https://github.com/reusee/gtk-go/blob/master/funcspec/funcspec.cpp

class TopLevelDeclConsumer : public clang::ASTConsumer
{
	public:
		TopLevelDeclConsumer()
			: clang::ASTConsumer()
		{}

		virtual ~TopLevelDeclConsumer()
		{}

		virtual bool HandleTopLevelDecl(clang::DeclGroupRef d)
		{
			for (clang::DeclGroupRef::iterator it = d.begin(); it != d.end(); it++) {
				clang::CXXRecordDecl * d = llvm::dyn_cast<clang::CXXRecordDecl>(*it);
				if (!d) {
					continue;
				}
				std::cout << std::endl;
				std::cout << "class " << d->getDeclName().getAsString() << std::endl;
			}
			return true;
		}
};

class TestConsumer : public clang::ASTConsumer
{
	private:
		void dump_base_classes(clang::CXXRecordDecl * record) const
		{
			if (record->getNumBases() == 0)
				return;

			std::cout << "  bases:" << std::endl;
			for (auto i = record->bases_begin(); i != record->bases_end(); ++i) {
				clang::CXXRecordDecl * rd = i->getType().getTypePtr()->getAsCXXRecordDecl();
				if (rd) {
					std::cout << "    " << rd->getQualifiedNameAsString() << std::endl;
				} else if (i->getType().getBaseTypeIdentifier()) {
					std::cout << "    (" << i->getType().getBaseTypeIdentifier()->getNameStart()
						<< ")" << std::endl;
				} else {
					std::cout << "    ((" << i->getType().getTypePtr()->getTypeClassName()
						<< "))" << std::endl;
				}
			}
		}

		void dump_function_parameters(clang::FunctionDecl * func) const
		{
			if (func->param_size() == 0)
				return;

			for (auto i = func->param_begin(); i != func->param_end(); ++i) {
				clang::QualType param_type = (*i)->getOriginalType();
				clang::IdentifierInfo * info = (*i)->getIdentifier();
				std::cout << "      " << param_type.getAsString();
				if (info)
					std::cout << " " << info->getName().str();
				std::cout << std::endl;
			}
		}

		void dump_methods(clang::CXXRecordDecl * record) const
		{
			if (record->method_begin() == record->method_end())
				return;

			std::cout << "  methods:" << std::endl;
			for (auto i = record->method_begin(); i != record->method_end(); ++i) {
				std::cout << "    "
					<< i->getDeclName().getAsString()
					<< " -> "
					<< i->getResultType().getAsString()
					<< std::endl;
				dump_function_parameters(*i);
			}
		}

		void dump_fields(clang::RecordDecl * record) const
		{
			if (record->field_begin() == record->field_end())
				return;

			std::cout << "  fields:" << std::endl;
			for (auto i = record->field_begin(); i != record->field_end(); ++i) {
				std::cout << "    " << i->getNameAsString() << std::endl;
			}
		}
	private:
		clang::ASTContext * context;
	public:
		explicit TestConsumer(clang::ASTContext * context)
			: clang::ASTConsumer()
			, context(context)
		{}

		virtual ~TestConsumer()
		{}

		virtual void HandleTagDeclDefinition(clang::TagDecl * tag)
		{
			clang::CXXRecordDecl * d = llvm::dyn_cast<clang::CXXRecordDecl>(tag);
			if (!d)
				return;

			clang::SourceLocation loc = d->getLocStart();
			clang::FullSourceLoc floc = context->getFullLoc(loc);
			clang::FileID fileid = floc.getFileID();

			std::string filename = floc.getManager().getFilename(loc).str();

			// no outputs for system headers
			if (floc.isValid() && floc.isInSystemHeader())
				return;

			std::cout << std::endl;
			std::cout << "class " << d->getQualifiedNameAsString();
			if (floc.isValid()) {
				std::cout << "  (defined at "
					<< filename
					<< ":"
					<< floc.getSpellingLineNumber()
					<< ")";
			}
			std::cout << std::endl;
			dump_base_classes(d);
			dump_methods(d);
			dump_fields(d);
		}
};


class NamespaceRecursiveASTVisitor : public clang::RecursiveASTVisitor<NamespaceRecursiveASTVisitor>
{
	private:
		void collect_namespace_hierarchy(clang::DeclContext * d, std::ostringstream & os) const;
	public:
		bool VisitNamespaceDecl(clang::NamespaceDecl * d);
};

void NamespaceRecursiveASTVisitor::collect_namespace_hierarchy(
		clang::DeclContext * d,
		std::ostringstream & os) const
{
	using namespace clang;

	if (!d)
		return;

	if (d->getDeclKind() != Decl::Namespace)
		return;

	NamespaceDecl * nsd = dyn_cast<NamespaceDecl>(d);
	if (!nsd)
		return; // should not happen

	collect_namespace_hierarchy(d->getParent(), os);
	os << "::" << nsd->getName().str();
}

bool NamespaceRecursiveASTVisitor::VisitNamespaceDecl(clang::NamespaceDecl * d)
{
	clang::DeclContext * context = d;
	std::ostringstream os;
	collect_namespace_hierarchy(d, os);
	std::cout
		<< context->getDeclKindName() << ": "
		<< os.str()
		<< std::endl;
	return true;
}

class NamespaceASTConsumer : public clang::ASTConsumer
{
	public:
		virtual bool HandleTopLevelDecl(clang::DeclGroupRef d);
};

bool NamespaceASTConsumer::HandleTopLevelDecl(clang::DeclGroupRef d)
{
	NamespaceRecursiveASTVisitor visitor;
	for (clang::DeclGroupRef::iterator b = d.begin(), e = d.end(); b != e; ++b) {
		visitor.TraverseDecl(*b);
	}
	return true;
}



class TestFrontendAction : public clang::ASTFrontendAction
{
	public:
		virtual clang::ASTConsumer * CreateASTConsumer(
				clang::CompilerInstance & compiler,
				llvm::StringRef filename)
		{
//			return new TestConsumer(&compiler.getASTContext());
			return new NamespaceASTConsumer;
		}
};

class TestFrontendActionFactory : public clang::tooling::FrontendActionFactory
{
	public:
		virtual clang::FrontendAction * create()
		{
			return new TestFrontendAction;
		}
};

int main(int argc, const char ** argv)
{
	clang::tooling::CommonOptionsParser options(argc, argv);
	clang::tooling::ClangTool tool(options.GetCompilations(), options.GetSourcePathList());
	return tool.run(new TestFrontendActionFactory);
}


#ifndef __CLANG__HPP__
#define __CLANG__HPP__

#include <clang-c/Index.h>
#include <string>

class Clang
{
	public:

		static std::string to_string(CXString str);

		static std::string getCursorSpelling(CXCursor cursor);
		static std::string getCursorKindSpelling(CXCursor cursor);

		static CXTranslationUnit parseTranslationUnit(
				CXIndex index,
				const std::string & source_filename,
				const char *const * command_line_args,
				int num_command_line_args,
				struct CXUnsavedFile * unsaved_files,
				unsigned num_unsaved_files,
				unsigned options);

		static void disposeTranslationUnit(CXTranslationUnit tu);

		static CXCursor getTranslationUnitCursor(CXTranslationUnit tu);

		static unsigned int getNumDiagnostics(CXTranslationUnit tu);
		static CXDiagnostic getDiagnostic(CXTranslationUnit tu, unsigned int i);
		static std::string formatDiagnostic(CXDiagnostic diag);
		static std::string formatDiagnostic(CXDiagnostic diag, unsigned int options);

		static CXCursorKind getCursorKind(CXCursor cursor);
		static std::string getCursorUSR(CXCursor cursor);
		static CXCursor getCursorLexicalParent(CXCursor cursor);
		static CXSourceLocation getCursorLocation(CXCursor cursor);

		static int getNumArguments(CXCursor cursor);

		static bool isInSystemHeader(CXSourceLocation location);

		static std::string getFileName(CXFile file);
};

#endif

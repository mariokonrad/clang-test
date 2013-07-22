#include "Clang.hpp"

std::string Clang::to_string(CXString str)
{
	const char * tmp = clang_getCString(str);
	if (tmp) {
		std::string translated = std::string(tmp);
		clang_disposeString(str);
		return translated;
	}
	return "";
}

std::string Clang::getCursorSpelling(CXCursor cursor)
{
	return to_string(clang_getCursorSpelling(cursor));
}

std::string Clang::getCursorKindSpelling(CXCursor cursor)
{
	return to_string(clang_getCursorKindSpelling(clang_getCursorKind(cursor)));
}

CXTranslationUnit Clang::parseTranslationUnit(
		CXIndex index,
		const std::string & source_filename,
		const char * const * command_line_args,
		int num_command_line_args,
		struct CXUnsavedFile * unsaved_files,
		unsigned num_unsaved_files,
		unsigned options)
{
	return clang_parseTranslationUnit(
		index, source_filename.c_str(),
		command_line_args, num_command_line_args,
		0, 0,
		CXTranslationUnit_None);
}

void Clang::disposeTranslationUnit(CXTranslationUnit tu)
{
	clang_disposeTranslationUnit(tu);
}

CXCursor Clang::getTranslationUnitCursor(CXTranslationUnit tu)
{
	return clang_getTranslationUnitCursor(tu);
}

unsigned int Clang::getNumDiagnostics(CXTranslationUnit tu)
{
	return clang_getNumDiagnostics(tu);
}

CXDiagnostic Clang::getDiagnostic(CXTranslationUnit tu, unsigned int i)
{
	return clang_getDiagnostic(tu, i);
}

std::string Clang::formatDiagnostic(CXDiagnostic diag)
{
	return formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions());
}

std::string Clang::formatDiagnostic(CXDiagnostic diag, unsigned int options)
{
	return to_string(clang_formatDiagnostic(diag, options));
}

CXCursorKind Clang::getCursorKind(CXCursor cursor)
{
	return clang_getCursorKind(cursor);
}

std::string Clang::getCursorUSR(CXCursor cursor)
{
	return to_string(clang_getCursorUSR(cursor));
}

CXCursor Clang::getCursorLexicalParent(CXCursor cursor)
{
	return clang_getCursorLexicalParent(cursor);
}

int Clang::getNumArguments(CXCursor cursor)
{
	return clang_Cursor_getNumArguments(cursor);
}

CXSourceLocation Clang::getCursorLocation(CXCursor cursor)
{
	return clang_getCursorLocation(cursor);
}

bool Clang::isInSystemHeader(CXSourceLocation location)
{
	return clang_Location_isInSystemHeader(location);
}

std::string Clang::getFileName(CXFile file)
{
	return to_string(clang_getFileName(file));
}


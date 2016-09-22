// From DigiPen CS391 "Better_assert" sample project by Cody Pritchard

#include <Stdafx.h>

// Assert Helper Function
// This combines all of the strings
// and displays the Message Box
// It returns true if the user wants to debug the assert
bool MyBetterAssert(const char* expression, const char* description, const char* file, int line, const char* funct) 
{
	std::ostringstream message;
	message << "ASSERTION FAILURE:" << std::endl;
	message << "File: " << file << std::endl;
	message << "Line: " << line << std::endl;
	message << "Function: " << funct << std::endl;
	message << std::endl;
	message << "Expression: " << expression << std::endl;

	if (description) {
		message << std::endl;
		message << "Description: " << description << std::endl;
	}

	message << std::endl;
	message << "IGNORE: Ignore the assert and continue" << std::endl;
	message << "RETRY: Break into the debugger" << std::endl;
	message << "ABORT: Exit immediately" << std::endl;

	int ret = MessageBoxA(NULL, message.str().c_str(), "ASSERTION FAILURE", MB_TASKMODAL | MB_SETFOREGROUND | MB_ABORTRETRYIGNORE | MB_ICONERROR);

	// Ignore
	if (ret == IDIGNORE) { return false; }

	// Debug
	if (ret == IDRETRY) { return true; }

	// Quit
	_exit(EXIT_FAILURE);
	//return true; // Never Reached
}
// From DigiPen CS391 "Better_assert" sample project by Cody Pritchard

#pragma once

//#include <cassert>
//#include <sstream>
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>

// Helper Macro
#define MY_ASSERT_STRINGIZE_INTERNAL(x) #x
#define MY_ASSERT_STRINGIZE(x) MY_ASSERT_STRINGIZE_INTERNAL(x)

// Getting more information from assert()
// See: "Squeezing More Out of Assert" by Steve Rabin
// in Game Programming Gems 1

// Trick 1: Assert with a message
#define ASSERT_MSG(x, msg) assert((x) && (msg))

// Trick 2: Write your own assert
#define MY_ASSERT(x) \
__pragma(warning(push)) \
__pragma(warning(disable:4127)) \
do { if (!(x)) { \
	int ret = MessageBoxA(NULL, MY_ASSERT_STRINGIZE(x), "ASSERTION FAILURE", MB_TASKMODAL | MB_SETFOREGROUND | MB_ABORTRETRYIGNORE | MB_ICONERROR); \
	if (ret == IDRETRY) { \
		DebugBreak(); \
		} else if (ret == IDABORT) { \
		_exit(EXIT_FAILURE); \
		} \
} } while(0) \
__pragma(warning(pop))

// Trick 3: Lets add lots of information
#define MY_BETTER_ASSERT(x, msg) \
__pragma(warning(push)) \
__pragma(warning(disable:4127)) \
do { if (!(x)) { \
	if (MyBetterAssert(MY_ASSERT_STRINGIZE(x), msg, __FILE__, __LINE__, __FUNCSIG__)) { \
		DebugBreak(); \
		} \
} } while(0) \
__pragma(warning(pop))

// Trick 4: Debug values as well
#define MY_ASSERT_WITH_VALUES(x, msg, ...) \
__pragma(warning(push)) \
__pragma(warning(disable:4127)) \
do { if (!(x)) { \
	static char buffer[256]; \
	_snprintf_s(buffer, _TRUNCATE, msg, ##__VA_ARGS__); \
	if (MyBetterAssert(MY_ASSERT_STRINGIZE(x), buffer, __FILE__, __LINE__, __FUNCSIG__)) { \
		DebugBreak(); \
		} \
} } while(0) \
__pragma(warning(pop))

bool MyBetterAssert(const char* expression, const char* description, const char* file, int line, const char* funct);
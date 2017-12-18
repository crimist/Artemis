#pragma once

#ifdef DEBUG

#include <stdarg.h>

void _printd(const char *file, const char *func, const int line, const char *fmt, ...);

clang_ignore("-Wgnu-zero-variadic-macro-arguments")
#define printd(...) _printd(__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__) // [proc_init()->src/process.c:38] Root access
clang_pop()

#else /* DEBUG */

#define printd(...)

#endif /* DEBUG */

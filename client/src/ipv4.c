#include "../include/main.h"

int8_t ipv4_cidrsplit(const char *string, uint8_t *a, uint8_t *b, uint8_t *c, uint8_t *d, uint8_t *mask) {
	/* Split an IP/CDIR into ints with lots of error checking */
	/* Returns failure location on fail and 0 on sucess */

	if (*string == NULL)
		return -1;

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

ipv4_t ipv4_pack(const uint8_t octet1, const uint8_t octet2, const uint8_t octet3, const uint8_t octet4) {
	return (((uint32_t) octet4) << 24) | (((uint32_t) octet3) << 16) | (((uint32_t) octet2) << 8) | ((uint32_t) octet1);
}

bool ipv4_getinfo(ipv4_t *ip) {
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
}

char *ipv4_unpack(const ipv4_t packed) {
	uint8_t tmp[4] = {0};
	char *str = calloc(16, sizeof(char *));

	tmp[3] = (uint8_t)(packed);
	tmp[2] = (uint8_t)(packed >> 8);
	tmp[1] = (uint8_t)(packed >> 16);
	tmp[0] = (uint8_t)(packed >> 24);

	snprintf(str, 16, "%d.%d.%d.%d", tmp[3], tmp[2], tmp[1], tmp[0]);
	return str;
}

uint32_t ipv4_mask(const int prefix_size) {
	if (prefix_size > 31)
	   return (uint32_t) 0xFFFFFFFFU;
	else if (prefix_size > 0)
	   return ((uint32_t) 0xFFFFFFFFU) << (32 - prefix_size);
	else
	   return (uint32_t) 0U;
}

bool ipv4_inrange(const ipv4_t ip, ipv4_t start, const ipv4_t finish) {
	while (start <= finish) {
		if (start == ip)
			return true;
		start++;
	}
	return false;
}

ipv4_t ipv4_random_public(void) {
	uint8_t ipState[4] = {0};

	do {
		ipState[0] = rand() % 255;
		ipState[1] = rand() % 255;
		ipState[2] = rand() % 255;
		ipState[3] = rand() % 255;
	}
	while	((ipState[0] == 0) 	||			  				// 0.0.0.0/8
			(ipState[0] == 127)	||			  				// 127.0.0.0/8
			(ipState[0] == 10)	||			  				// 10.0.0.0/8
			((ipState[0] == 192) && (ipState[1] == 168)) 	// 192.168.0.0/16
			);

	return ipv4_pack(ipState[0], ipState[1], ipState[2], ipState[3]);
}

ipv4_t ipv4_packstr(const char *ip) {
	uint8_t octet[6] = {0};

	if (ipv4_cidrsplit(ip, &octet[1], &octet[2], &octet[3], &octet[4], &octet[5]))
		return 0;

	return ipv4_pack(octet[1], octet[2], octet[3], octet[4]);
}

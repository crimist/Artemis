#include "../include/main.h"

void rand_init()
{
	srand(time(NULL) ^ getpid()); // Random seed of current time ^ process id so very random
	return;
}

uint32_t rand_uint()
{
	return 0;
}
char *rand_string(const int len)
{
	register int i;
	char *tmp = calloc(len + 1, sizeof(char *));

	for (i = 0; i < len; i++)
		tmp[i] = (rand() % (91 - 65)) + 65;

	tmp[len + 1] = 0;

	return tmp;
}

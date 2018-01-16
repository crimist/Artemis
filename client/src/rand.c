#include "../include/main.h"

void randInit(void) {
	srand((uint)time(NULL) ^ (uint)getpid()); // Random seed of current time ^ process id so very random
	return;
}

char *rand_string(uint len) {
	char *tmp = calloc(len + 1, sizeof(char *));

	for (uint i = 0; i < len; i++)
		tmp[i] = (rand() % (91 - 65)) + 65;

	tmp[len + 1] = 0;

	return tmp;
}

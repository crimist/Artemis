#include "../include/main.h"

/* Just to be 100% sure that we don't accidently put it in a proper build */
#ifdef DEBUG

void _printd(const char *file, const char *func, const int line, const char *fmt, ...) {
	printf("[%s->%s:%d] ", func, file, line);

	va_list args;
	va_start(args, fmt);
	int ret = vfprintf(stdout, fmt, args);
	va_end(args);

	printf("\n");

	if (ret < 0) {
		fprintf(stderr, "Print Debug Failed!\nExiting...\n");
		exit(EXIT_FAILURE);
	}
	return;
}

#endif /* DEBUG */

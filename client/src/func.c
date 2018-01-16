#include "../include/main.h"

/* read until end or newline */
unsigned char *fdgets(unsigned char *buffer, int bufferSize, int fd) {
	int got = 1, total = 0;

	while (got == 1 && total < bufferSize && *(buffer + total - 1) != '\n') {
		got = read(fd, buffer + total, 1);
		total++;
	}
	return got == 0 ? NULL : buffer;
}

/* get the length of the string */
size_t func_strlen(const char *str) {
	int i = 0;
	while (*str++ != 0)
		i++;
	return i;
}

/* zero out a peice of memory */
void func_zero(void *buf, int len) {
	char *zero = buf;
	while (len--)
		*zero++ = 0;
	return;
}

/* change a string to upper case */
void func_strupr(char *buf, int len) {
	while (len--) {
		*buf = toupper(*buf);
		buf++;
	}
	return;
}

/* find a substring in a string */
int func_substr(char *buf, int buflen, char *find, int findlen) {
	int match = 0;

	for (int i = 0; i < buflen; i++) {
		if (buf[i] == find[match])
			if (++match == findlen)
					return i + 1;
		else
			match = 0;
	}
	return -1;
}

/* substr but it ignores case */
int func_subcasestr(const char *buf, const char *find)
{
	int findlen = strlen(find), buflen = strlen(buf), match = 0;

	for (int i = 0; i < buflen; i++) {
		if (toupper(buf[i]) == toupper(find[match])) {
			if (++match == findlen)
				return i + 1;
		}
		else
			match = 0;
	}
	return -1;
}

bool func_substrs(const char *str, const char **strs) {
	if (NULL == str || NULL == strs)
		return false;

	while (*strs) {
		if (subcasestr(str, *strs++) != -1)
			return true;
	}
	return false;
}

/*
bool func_strcmp(char *str1, char *str2)
{
		int l1 = func_strlen(str1), l2 = func_strlen(str2);

		if (l1 != l2)
				return FALSE;

		while (l1--)
		{
				if (*str1++ != *str2++)
						return FALSE;
		}

		return TRUE;
}
*/

/* Compare two strings with a length to compare to */
bool func_strncmp(char *str1, char *str2, int len) {
	int l1 = strlen(str1), l2 = strlen(str2);

	if (l1 < len || l2 < len)
		return false;

	while (len--)
	{
		if (*str1++ != *str2++)
			return false;
	}

	return true;
}

/* strncmp but ignores case */
bool func_strcasencmp(char *str1, char *str2, int len) {
	int l1 = strlen(str1), l2 = strlen(str2);

	if (l1 < len || l2 < len)
		return false;

	while (len--) {
		if (toupper(*str1++) != toupper(*str2++))
			return false;
	}

	return true;
}

/* strcat/strncat except it'll accept arguments just like printf */
void func_append(char *buf, const int len, const char *fmt, ...) {
	va_list list;
	va_start(list, fmt);
	vsnprintf(buf, len, fmt, list);
	va_end(list);
}

/* Explode from PHP */
int8_t func_explode(const char *split, const char *delim, char ***list) {
	// Check for invalid pointers
	if (split == NULL || delim == NULL || list == NULL)
		return -1;

	// Create poiners ect.
	char * str;
	char * token;
	char **templist = (char **)realloc(NULL, sizeof(*templist));
	char **temptemplist;
	int i = 0;

	// Allocate string to split
	if ((str = strdup(split)) == NULL)
		return -1;

	// Initlize token
	if ((token = strtok(str, delim)) == NULL)
		return -1;

	while (token != NULL) {
		temptemplist = realloc(templist, (sizeof *templist) * (i + 1));
		templist = temptemplist;

		templist[i] = strdup(token);

		i++;
		token = strtok(NULL, delim);
	}

	free(str);
	// #warning this may cause erros?
	free(temptemplist);
	*list = templist;
	return i;
}

/* Free an exploded list */
void func_freelist(char ***list, int len) {
	// printd("%p\n", (void *) list);
	for (int x = 0; x < len; x++)
	{
		// printd("[%d] %p\n", x, (void *) list[x]);
		free(list[x]);
	}
	free(list);
	return;
}

/*
How to call ^
int example()
{
	char **list;
	int size = explode("this.is.a.string", ".", &list);
	for(int i = 0; i < size; i++)
		printf("[%d] %s\n", i, list[i]);
	freelist(list, size);
	return 0;
}
*/

/* readuntil we find a string */
bool func_readuntil(int fd, char *buffer, int buflen, const char **toFind) {
	int got = 1, total = 0;
	while (got == 1 && total < buflen) {
		got = read(fd, buffer + total, 1);
		total++;
		if (substrs(buffer, toFind) == true)
			return true;
	}
	return false;
}

/* Count number of times a char is found in a string */
int func_countChar(const char *string, char ch) {
	int count = 0;
	int length = strlen(string);

	for (int i = 0; i < length; i++) {
		if (string[i] == ch)
			count++;
	}

	return count;
}

/*
Read /proc/sys/file-max or if linux 4.4.0 /proc/sys/fs/file-max and record its
value of max fds
Raed /proc/sys/fs/file-nr to see how many are open right now

Use https://linux.die.net/man/2/uname to get sys info
*/

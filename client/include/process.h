#pragma once

bool proc_root(void);
void proc_init(void);
pid_t proc_add(uint8_t type, char *param);
// bool proc_remove(int);
uint8_t proc_killall(void);

#ifdef DEBUG
void proc_dump(void);
#endif

#pragma once

bool proc_root(void);
void proc_init(void);
int proc_add(int type, char *param);
// bool proc_remove(int);
uint8_t proc_killall(void);

#ifdef DEBUG
void proc_dump(void);
#endif

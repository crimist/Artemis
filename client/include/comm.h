#pragma once

#define COMM_ADDR "45.32.89.146"
#define COMM_PORT 80
#define COMM_TIMEOUT 120 // Time between posts
#define COMM_KEY "KEY"

void comm_init(void);
bool comm_comm(void);

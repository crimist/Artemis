#pragma once

int8_t ipv4_cidrsplit(const char *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *);
ipv4_t ipv4_pack(const uint8_t, const uint8_t, const uint8_t, const uint8_t);
bool ipv4_getinfo(ipv4_t *);
char * ipv4_unpack(const ipv4_t);
uint32_t ipv4_mask(const int);
bool ipv4_inrange(const ipv4_t, ipv4_t, const ipv4_t);
ipv4_t ipv4_random_public(void);
ipv4_t ipv4_packstr(const char *);

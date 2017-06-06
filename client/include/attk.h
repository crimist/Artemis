#pragma once

#ifdef WEAPONIZED
CLANG_WRN("Weaponized build being built")
void attk_http(uint32_t, uint16_t, uint8_t, uint16_t, char *);
#endif

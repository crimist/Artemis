#pragma once

#define strlen func_strlen
#define zero func_zero
#define strupr func_strupr
#define substr func_substr
#define subcasestr func_subcasestr
#define substrs func_substrs
#undef strncmp
#define strncmp func_strncmp
#define strcasencmp func_strcasencmp
#define append func_append
#define explode func_explode
#define freelist func_freelist
#define readuntil func_readuntil
#define countChar func_countChar

unsigned char *fdgets(unsigned char *, int, int);
size_t func_strlen(const char *);
void func_zero(void *, int);
void func_strupr(char *, int);
int func_substr(char *, int, char *, int);
int func_subcasestr(const char *, const char *);
bool func_substrs(const char *str, const char **strs);
bool func_strncmp(char *, char *, int);
bool func_strcasencmp(char *, char *, int);
void func_append(char *, const int, const char *, ...);
int8_t func_explode(const char *, const char *, char ***);
void func_freelist(char ***, int);
bool func_readuntil(int, char *, int, const char **);
int func_countChar(const char *, char);

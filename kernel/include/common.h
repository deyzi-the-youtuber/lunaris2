#ifndef __COMMON_H
#define __COMMON_H

#include <sys/types.h>
#include <stdarg.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef enum 
{
  true = 1,
  false = 0
}bool;

// string struff
char * itoa(int num, char * str, int base);
void reverse(char * str, size_t sz);
ssize_t strlen(const char * str);
char * strcat(char * dest, const char * src);
void strcpy(char * dest, char * src);
// memory stuff
void * memset(void * dest, char val, size_t count);
void * memcpy(void * dest, const void * src, size_t count);
void * memmove(void * dest, const void * src, size_t n);
// printing stuff
ssize_t vsprintf(const char fmt[], va_list arg);

#endif

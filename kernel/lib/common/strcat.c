#include <common.h>

char * strcat(char * dest, const char * src)
{
  char * rdest = dest;
  while (*dest)
    dest++;
  while (*dest++ = *src++);
  return rdest;
}

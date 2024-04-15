#include <common.h>

ssize_t strlen(const char * str)
{
  int i = 0;
  while(str[i]) i++;
  return i;
}

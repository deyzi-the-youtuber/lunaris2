#include <common.h>
#include <sys/types.h>
#include <stdint.h>

void * memset(void * dest, char val, size_t count)
{
  char * temp = (char *)dest;
  for(; count; count--) *temp++ = val;
  return dest;
}

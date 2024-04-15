#include <common.h>

char * itoa(int num, char * str, int base)
{
  int i = 0;
  int neg = 0;

  if (num == 0)
  {
    str[i++] = '0';
    str[i] = '\0';
    return str;
  }

  if (num < 0 && base == 10)
  {
    neg = 1;
    num = -num;
  }

  while (num != 0)
  {
    int rem = num % base;
    str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
    num = num / base;
  }
 
  if (neg)
    str[i++] = '-';
 
  str[i] = '\0';

  reverse(str, i);
 
  return str;
}

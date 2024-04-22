#include <common.h>
#include <stdarg.h>
#include <lunaris/memory.h>

// i hate coding vasprintf
char * vasprintf(const char fmt[], va_list ap)
{
  va_list tmp_ap = ap;
  int fmt_i;
  size_t fmt_len = 0;
  for(fmt_i = 0; fmt[fmt_i]; fmt_i++)
  {
    if(fmt[fmt_i] == '%')
    {
      fmt_i++;
      switch (fmt[fmt_i])
      {
        case 's':
          char * str = va_arg(tmp_ap, char *);
          fmt_len += strlen(str);
          break;

        case 'c':
        case 'd':
          fmt_len++;
          break;
        case 'x':
          fmt_len += 2;
          break;
        default:
          break;
      }
    }
    else 
    {
      fmt_len++;
    }
  }
  va_end(tmp_ap);
  
  char * buf = (char *)kmalloc(fmt_len + 1);
  if (!buf)
    return NULL;
  int buf_i = 0;

  for(fmt_i = 0; fmt[fmt_i]; fmt_i++)
  {
    if (fmt[fmt_i] == '%')
    {
      fmt_i++;
      switch (fmt[fmt_i])
      {
        case 's':
          char * str = va_arg(ap, char *);
          for(int i = 0; str[i]; i++)
          {
            buf[buf_i] = str[i];
            buf_i++;
          }
          break;
        case 'd':
          int num = va_arg(ap, int);
          char nums[255];
          itoa(num, nums, 10); // num-nums :)
          for(int i = 0; nums[i]; i++)
          {
            buf[buf_i] = nums[i];
            buf_i++;
          }
          break;
        case 'x':
          int hex = va_arg(ap, int);
          char hexs[255];
          itoa(hex, hexs, 16);
          for(int i = 0; hexs[i]; i++)
          {
            buf[buf_i] = hexs[i];
            buf_i++;
          }
          break;
        case 'c':
          char ch = (char)va_arg(ap, int);
          buf[buf_i] = ch;
          buf_i++;
          break;
        default:
          break;
      }
    }
    else
    {
      buf[buf_i] = fmt[fmt_i];
      buf_i++;
    }
  }
  buf[buf_i] = '\0';
  return buf;
}

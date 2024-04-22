#include <common.h>
#include <lunaris/video.h>
#include <sys/types.h>
#include <stdarg.h>
#include <stdint.h>
#include <lunaris/sync.h>

ssize_t vprintf(const char fmt[], va_list arg)
{
  size_t width = 0;
  size_t ret = 0;
  for(int i = 0; fmt[i]; i++)
  {
    if(fmt[i] == '%')
    {
      i++;
      if(fmt[i] >= '0' && fmt[i] <= '9')
      {
        width = fmt[i] - '0';
        i++;
      }
      switch (fmt[i]) 
      {
        case 's':
          char * str = va_arg(arg, char *);
          ret += strlen(str);
          if(str)
            video_puts(str);
          else
            video_puts("(null)");
          break;
        case 'd':
          int num = va_arg(arg, int);
          char nums[20];
          itoa(num, nums, 10);
          ret += strlen(nums);
          video_puts(nums);
          break;
        case 'x':
          uint32_t hex = va_arg(arg, int);
          char hexs[20];
          itoa(hex, hexs, 16);
          ret += strlen(hexs);
          video_puts(hexs);
          break;
        case 'c':
          char c = va_arg(arg, int);
          ret++;
          video_putc(c);
          break;
        default:
          break;
      }
    }
    else 
    {
      video_putc(fmt[i]);
      ret++;
    }
  }
  return ret;
}


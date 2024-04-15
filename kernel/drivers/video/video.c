#include <lunaris/video.h>
#include <multiboot2.h>
#include <stdint.h>
#include <common.h>

struct video_driver * driver = {0};
extern struct video_driver * __fb_init(struct multiboot_header_tag * tag, uintptr_t addr);

static struct video_driver * __video_init(struct multiboot_header_tag * tag, uintptr_t addr)
{
  // use default video thing
  return __fb_init(tag, addr);
}

int video_init(struct multiboot_header_tag * tag, uintptr_t addr)
{
  driver = __video_init(tag, addr);
  if(!driver)
    return -1;
  return 0;
}

void video_putc(const char c) 
{
  char str[2];
  str[0] = c;
  str[1] = 0;
  driver->ops->write(str, 1);
}

void video_puts(const char fmt[])
{
  ssize_t len = strlen(fmt);
  driver->ops->write(fmt, len);
}

#include <sys/types.h>
#include <multiboot2.h>

#ifndef __VIDEO_H
#define __VIDEO_H

struct video_driver_ops
{
  void (*write)(const char *, ssize_t sz);
  int  (*remap)(int, int);
};

struct video_driver
{
  uint32_t fb_addr;
  uint32_t width, height;
  uint32_t bpp, pitch;
  int x, y;
  // defined by the driver using this struct
  struct video_driver_ops * ops;
};

int video_init(struct multiboot_header_tag * tag, uintptr_t addr);
void video_puts(const char fmt[]);
void video_putc(const char c);

#endif

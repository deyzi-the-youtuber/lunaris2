#include <sys/types.h>
#include <stdint.h>
#include <lunaris/video.h>
#include <common.h>
#include <sys/io.h>

void __vga_generic_write(const char fmt[], ssize_t sz);
int __vga_generic_remap(int x, int y);

static struct video_driver_ops vga_generic_ops =
{
  .write = __vga_generic_write,
  .remap = __vga_generic_remap
};

struct video_driver vga_generic =
{
  .fb_addr = (uintptr_t)0xb8000,
  .width  = 80,
  .height = 25,
  .bpp    = 16,
  .x      = 0,
  .y      = 0,
  .ops = &vga_generic_ops
};

static void __vga_generic_scroll(void)
{
  // Check if the cursor is at the last row of the VGA buffer
  if (vga_generic.y >= vga_generic.height)
  {
      // Move all rows up by one (excluding the first row)
    for (int i = 1; i < vga_generic.height; i++)
    {
      memcpy((void *)vga_generic.fb_addr + (i - 1) * vga_generic.width * 2, (void *)vga_generic.fb_addr + i * vga_generic.width * 2, vga_generic.width * 2);
    }

    // Clear the last row
    char * last_row = (void *)vga_generic.fb_addr + 24 * vga_generic.width * 2;
    memset(last_row, 0, vga_generic.width * 2);

    // Move the cursor up by one row
    vga_generic.y--;
  }
}

static void __vga_generic_putc(uint8_t ch)
{
  uint16_t position = vga_generic.y * vga_generic.width + vga_generic.x;
  uint16_t * vmem = (uint16_t *)(vga_generic.fb_addr + position * 2);
  switch(ch)
  {
    case '\n':
    {
      vga_generic.x = 0;
      vga_generic.y++;
      goto remap;
      break;
    }
    case '\b':
    {
      break;
    }
    default:
    {
      *vmem = (uint16_t)ch | (0x7 << 0x8);
    }
  }
  vga_generic.x++;
remap:
  __vga_generic_remap(vga_generic.x, vga_generic.y);
  __vga_generic_scroll();
}

void __vga_generic_write(const char fmt[], ssize_t sz)
{
  for(int i = 0; i < sz; i++)
  {
    __vga_generic_putc((uint8_t)fmt[i]);
  }
}

int __vga_generic_remap(int x, int y)
{
  uint16_t position = y * vga_generic.width + x;
  outb(0x3D4, 0xf);
  outb(0x3D5, (uint8_t)(position & 0xff));
  outb(0x3D4, 0xe);
  outb(0x3D5, (uint8_t)(position >> 8) & 0xff);
  return (int)position;
}

struct video_driver * __vga_generic_init(void)
{
  return &vga_generic;
}

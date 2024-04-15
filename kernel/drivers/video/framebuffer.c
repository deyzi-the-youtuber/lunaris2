#include <multiboot2.h>
#include <font/8x8.h>
#include <sys/types.h>
#include <stdint.h>
#include <lunaris/video.h>
#include <common.h>

void __fb_write(const char fmt[], ssize_t sz);
int __fb_remap();

struct video_driver_ops fb_ops = {
  .write = __fb_write,
  .remap = __fb_remap
};

struct video_driver fb;

static void __fb_putpx(int x, int y, int r, int g, int b)
{
  uint8_t * buf = (uint8_t *)fb.fb_addr;
  uint32_t offset =
      (x + y * fb.width) *
      4;

  buf[offset] = b;
  buf[offset + 1] = g;
  buf[offset + 2] = r;
  buf[offset + 3] = 0;
}

static void __fb_print_glyph(uint8_t * glyph)
{
  int x = fb.x * 8;
  int y = fb.y * 8;
  for (int dy = 0; dy < 8; dy++)
  {
    for (int dx = 0; dx < 8; dx++)
    {
      int color = (glyph[dy] >> dx) & 1;
      __fb_putpx(x + dx, y + dy, color ? 170 : 0, color ? 170 : 0, color ? 170 : 0);
    }
  }
}



// Function to scroll the framebuffer when the cursor reaches the bottom
static void __fb_scroll()
{
  if (fb.y > (fb.height / 8) - 1) // Check if the cursor is at the last row
  {
    // Calculate the size of a single row in bytes
    size_t row_size_bytes = fb.width * 4 * 8;

    // Calculate the size of all rows except the last one
    size_t all_rows_except_last_size = (fb.height / 8) * row_size_bytes;

    // Move all rows up by one (excluding the first row)
    memmove((uint8_t *)fb.fb_addr,
          (uint8_t *)fb.fb_addr + row_size_bytes,
          all_rows_except_last_size);

    // Clear the last row
    memset((uint8_t *)fb.fb_addr + all_rows_except_last_size, 0, row_size_bytes);

    // Move the cursor up by one row
    fb.y--;
  }
}

static void __fb_putc(const char c)
{
  switch(c)
  {
    case '\n':
      fb.x = 0;
      fb.y++;
      __fb_scroll();
      return;
      break;
    case '\b':
      fb.x--;
      __fb_print_glyph(font8x8[(int)0]);
      return;
      break;
    default:
      break;
  }

  uint8_t * glyph = font8x8[(int)c];
  
  __fb_print_glyph(glyph);

  fb.x++;
  if (fb.x > fb.width / 8)
  {
    fb.x = 0;
    fb.y++;
  }
  __fb_scroll();
}

void __fb_write(const char fmt[], ssize_t sz)
{
  for(int i = 0; i < sz; i++)
  {
    __fb_putc(fmt[i]);
  }
}

struct video_driver * __fb_init(struct multiboot_header_tag * tag, uintptr_t addr)
{
  tag = (struct multiboot_header_tag *)(addr + 8);
  while(tag->type != MULTIBOOT_TAG_TYPE_END)
  {
    switch (tag->type)
    {
      case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
        struct multiboot_tag_framebuffer_common * new = 
          (struct multiboot_tag_framebuffer_common *)tag;
        fb.fb_addr = new->framebuffer_addr;
        fb.height = new->framebuffer_height;
        fb.width = new->framebuffer_width;
        fb.bpp = new->framebuffer_bpp;
        break;        
      default:
        break;
    }
    tag = (struct multiboot_header_tag *)((uint8_t *) tag + ((tag->size + 7) & ~7));
  }
  fb.x = 0;
  fb.y = 0;
  fb.ops = &fb_ops;
  return &fb;
}

int __fb_remap(int x, int y)
{
  fb.x = x;
  fb.y = y;
  return x | y;
}

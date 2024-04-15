#include <multiboot2.h>
#include <stdint.h>
#include <common.h>
#include <lunaris/video.h>
#include <lunaris/printk.h>
#include <lunaris/memory.h>

extern uint32_t kernel_end;
struct multiboot_header_tag * mbi;

void kernel_main(uint32_t addr)
{
  if(addr & 7)
    return;
  mbi = (struct multiboot_header_tag *)addr;
  if (video_init(mbi, addr) < 0)
    return;
  // search for memory map
  struct multiboot_header_tag * tag = (struct multiboot_header_tag *)(addr + 8);
  while(tag->type != MULTIBOOT_TAG_TYPE_END)
  {
    switch (tag->type)
    {
      case MULTIBOOT_TAG_TYPE_MMAP:
        printk("E820 Memory Map:\n");
        struct multiboot_mmap_entry * mmap;
        struct multiboot_tag_mmap * new = 
          (struct multiboot_tag_mmap *)tag;
        for (mmap = new->entries; 
          (uint8_t*)mmap < (uint8_t*) tag + tag->size;
          mmap = (multiboot_memory_map_t *)((uint32_t)mmap + new->entry_size)) {
          printk("  [0x%x-0x%x] type: 0x%x\n", (uint32_t)mmap->addr, (uint32_t)mmap->addr + (uint32_t)mmap->len, mmap->type);
        }
        break;        
      default:
        break;
    }
    tag = (struct multiboot_header_tag *)((uint8_t *) tag + ((tag->size + 7) & ~7));
  }
  kheap_init(&kernel_end);
}

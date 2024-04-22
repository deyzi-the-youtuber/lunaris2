#include <multiboot2.h>
#include <stdint.h>
#include <common.h>
#include <lunaris/video.h>
#include <lunaris/printk.h>
#include <lunaris/memory.h>
#include <sys/utsname.h>
#include <lunaris/device.h>
#include <arch.h>
#include <lunaris/pci.h>

extern uint32_t kernel_end;
struct multiboot_header_tag * mbi;

void kernel_main(uint32_t addr)
{
  if(addr & 7)
    return;
  mbi = (struct multiboot_header_tag *)addr;
  if (video_init(mbi, addr) < 0)
    return;
  kheap_init(&kernel_end);
  // initialize kernel heap before anything

  printk("%s kernel version %s-%s (%s version %s)\n", 
      UTSNAME_SYSNAME, UTSNAME_RELEASE, UTSNAME_MACHINE, __CONFIG_COMPILER__, __CONFIG_COMPILER_VERSION__);
  arch_init();
  devices_init();
  pci_init();
  for(;;);
}

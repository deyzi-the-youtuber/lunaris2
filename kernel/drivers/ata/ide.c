#include <stdint.h>
#include <arch.h>
#include <lunaris/blkdev.h>
#include <sys/io.h>
#include <lunaris/pci.h>
#include <errno.h>
#include <lunaris/module.h>
#include <common.h>
#include <lunaris/printk.h>
#include <lunaris/memory.h>

#include "ata.h"

MODULE_NAME("ide")

static uint32_t bar0, bar1, bar2, bar3, bar4;

static struct pci_device * pci_ide;

static struct ide_drive drives[4];
static struct ide_channel channels[2];

static int num_drives = 0;

// linux-like pci driver setup
static const struct pci_id ata_ids[] = 
{
  PCI_ID(INTEL, 0x7010), // 82371SB PIIX3 IDE [Natoma/Triton II]
  PCI_ID(INTEL, 0x7111), // 82371AB/EB/MB PIIX4 IDE
  PCI_ID(0xffff, 0xffff)
};

static int __ide_probe(struct pci_device * pd)
{
  return (pd->subclass == 1 && pd->class == 1);
}

static int __ide_driver_attach(struct pci_device * dev)
{
  int rc = 0;
  rc = __ide_probe(dev);
  if(!rc)
  {
    printm("Incorrect classes\n");
    return -ENODEV;
  }

  // If bit 0 and 2 are not set in PROG_IF, this means that the drive is in compatibility mode
  // Because of that, we can use the default bar values.

  bar0 = (dev->prog_if & ATA_PRIM_LEGACY) ? pci_get_bar(dev->bus, dev->slot, dev->functions, 0) : ATA_BUS1_PRIMARY_IO_PORT;
  bar1 = (dev->prog_if & ATA_PRIM_LEGACY) ? pci_get_bar(dev->bus, dev->slot, dev->functions, 1) : ATA_BUS1_PRIMARY_CTRL_REG;
  bar2 = (dev->prog_if & ATA_PRIM_LEGACY) ? pci_get_bar(dev->bus, dev->slot, dev->functions, 2) : ATA_BUS1_SECOND_IO_PORT;
  bar3 = (dev->prog_if & ATA_PRIM_LEGACY) ? pci_get_bar(dev->bus, dev->slot, dev->functions, 3) : ATA_BUS1_SECONDARY_CTRL_REG;
  bar4 = pci_get_bar(dev->bus, dev->slot, dev->functions, 4);
  pci_ide = dev;
  return 0;
}

static void __ide_send_cmd(uint32_t bar, uint8_t cmd)
{
  outb(bar + 7, cmd);
}

static uint8_t __ide_read_status(uint32_t bar)
{
  return inb(bar + 7);
}

static void __ide_select_drive(uint32_t bar, uint8_t opt)
{
  outb(bar + 6, opt);
}

static void __ide_reset(uint32_t bar)
{
  // make sure it gets to 5
  for(int i = 2; i < 6; i++)
  {
    outw(bar + i, 0);
  }
}

static uint8_t __ide_read_err(uint32_t bar)
{
  inb(bar + 1);
}

static void __atapi_init()
{

}

static void __ide_init()
{
  int rc, retry = 0;
retry:;
  uint32_t bar = retry ? bar2 : bar0;
  __ide_send_cmd(bar, ATA_CMD_PACKET);
  __ide_select_drive(bar, retry ? ATA_SLAVE : ATA_MASTER); // master drive
  // reset sectorcount, and other things
  __ide_reset(bar);
  __ide_send_cmd(bar, ATA_CMD_IDENTIFY);
  rc = __ide_read_status(bar);
  if(!rc)
  {
    printm("Failed to initialize ATA\n");
    if(retry == 1)
      return;
    retry = 1;
    goto retry;
  }
  while((__ide_read_status(bar) & ATA_STATUS_BUSY));
loop:;
  rc = __ide_read_status(bar);
  if(rc & ATA_STATUS_ERR)
  {
    int rc2 = __ide_read_err(bar);
    if(rc2 != 4)
    {
      printm("ATA drive is in error mode. Code: 0x%x\n", rc2);
      return;
    }
    else
    {
      printm("ATA drive might be ATAPI...\n");
      if(inb(bar + 4) == 0x14 && inb(bar + 5) == 0xeb)
      {
        printm("Detected ATAPI drive.\n");
        return;
      }
      else
      {
        if(inb(bar + 4) == 0x3c && inb(bar + 5) == 0xc3)
        printm("Detected SATA drive.\n");
        return;
      }
      return;
    }
  }
  if(!(rc & ATA_STATUS_DRQ))
    goto loop;
  void * init_data = (void *)kmalloc(512);
  for(int i = 0; i < 256; i++)
    *(uint16_t *)(init_data + i * 2) = inw(bar);
  // get drive info
  drives[num_drives].used = true;
  drives[num_drives].signature = *((uint16_t *)(init_data + ATA_IDENT_DEVICETYPE));
  drives[num_drives].features = *((uint16_t *)(init_data + ATA_IDENT_CAPABILITIES));
  drives[num_drives].commands = *((uint32_t *)(init_data + ATA_IDENT_COMMANDSETS));
  if(drives[num_drives].commands & ATA_48BIT_ADDR)
  {
    drives[num_drives].size = *((uint32_t *)(init_data + ATA_IDENT_MAX_LBA_EXT));
  }
  else
  {
    drives[num_drives].size = *((uint32_t *)(init_data + ATA_IDENT_MAX_LBA));
  }

  for(int k = 0; k < 40; k += 2) 
  {
    drives[num_drives].name[k] = *(uint16_t *)(init_data + ATA_IDENT_MODEL + k + 1);
    drives[num_drives].name[k + 1] = *(uint16_t *)(init_data + ATA_IDENT_MODEL + k);
  }
  num_drives++;
  printm("Successfully initialized drive: %s\n", drives[num_drives - 1].name);
  kfree(init_data);
}

static void __ide_driver_init(void)
{
  __ide_init();
}

struct pci_driver ide_driver =
{
  .name = "ATA Controller",
  .ident = (void *)&ata_ids,
  .attach_driver = __ide_driver_attach,
  .init_driver = __ide_driver_init
};

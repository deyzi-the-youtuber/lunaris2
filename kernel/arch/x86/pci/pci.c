#include <stdint.h>
#include <lunaris/printk.h>
#include <lunaris/module.h>
#include <lunaris/device.h>
#include <sys/io.h>
#include <common.h>
#include <lunaris/memory.h>
#include <lunaris/pci.h>
#include <errno.h>

MODULE_NAME("pci")

#define PCI_CONFIG_ADDR 0xcf8
#define PCI_CONFIG_DATA 0xcfc

#define PCI_VENDOR_INVALID 0xffff

int pci_reg = 0;

// built-in drivers
extern struct pci_driver ide_driver;
extern struct pci_driver sata_driver;

static struct pci_device devices[32];
static struct pci_driver * drivers[] =
{
  // these are built in the kernel
  &ide_driver,
  &sata_driver,
  NULL
};

static uint16_t __pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
  uint32_t address;
  uint32_t lbus  = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;
  uint16_t tmp;

  address = (uint32_t)((lbus << 16) | (lslot << 11) |
    (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

  outl(PCI_CONFIG_ADDR, address);
  tmp = (uint16_t)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
  return tmp;
}

static uint32_t __pci_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
  uint32_t low = __pci_read_word(bus, slot, func, offset);
  uint32_t high = __pci_read_word(bus, slot, func, offset + 2);

  return (high << 16) | low;
}

static uint16_t __pci_get_vendor(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t vendor = __pci_read_word(bus, slot, func, 0);
  if (vendor == PCI_VENDOR_INVALID)
    return 0;
  return vendor;
}

static uint16_t __pci_get_device(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t device = __pci_read_word(bus, slot, func, 2);
  return device;
}

static uint8_t __pci_get_subclass(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t subclass = __pci_read_word(bus, slot, func, 10);
  return (subclass & ~0xff00);
}

static uint8_t __pci_get_class(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t class = __pci_read_word(bus, slot, func, 10);
  return (class & ~0x00ff) >> 8;
}

static uint8_t __pci_get_progif(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t progif = __pci_read_word(bus, slot, func, 8);
  return (progif & ~0x00ff) >> 8;
}

static uint8_t __pci_get_revision(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t revision = __pci_read_word(bus, slot, func, 8);
  return (revision & ~0xff00);
}

static uint8_t __pci_get_header(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t header = __pci_read_word(bus, slot, func, 14);
  return (header & 0xff);
}

uint32_t pci_get_bar(uint8_t bus, uint8_t slot, uint8_t func, uint8_t bar)
{
  uint8_t offset = 0x10 + (bar * 4);
  uint32_t rc = __pci_read_dword(bus, slot, func, offset);
  //printk("BAR%d: 0x%x\n", bar, rc);
  return rc;
}

static bool __pci_is_invalid(uint8_t bus, uint8_t slot, uint8_t func)
{
  return (__pci_get_vendor(bus, slot, func) == 0);
}

static void __pci_register_device(uint8_t bus, uint8_t slot, uint8_t func)
{
  devices[pci_reg].ident.ven = __pci_get_vendor(bus, slot, func);
  devices[pci_reg].ident.dev = __pci_get_device(bus, slot, func);
  devices[pci_reg].prog_if = __pci_get_progif(bus, slot, func);
  devices[pci_reg].subclass = __pci_get_subclass(bus, slot, func);
  devices[pci_reg].class = __pci_get_class(bus, slot, func);
  devices[pci_reg].bus = bus;
  devices[pci_reg].slot = slot;
  devices[pci_reg].functions = func;
  //printk("%x.%x.%x %x:%x\n", devices[pci_reg].class, devices[pci_reg].subclass, devices[pci_reg].prog_if, devices[pci_reg].ident.ven, devices[pci_reg].ident.dev);
  pci_reg++;
}

static void __pci_enumerate(void)
{
  memset(devices, 0, sizeof(devices));
  for (int bus = 0; bus < 256; bus++)
  {
    for (int slot = 0; slot < 32; slot++)
    {
      for(int func = 0; func < 8; func++)
      {
        if(__pci_is_invalid(bus, slot, func))
          continue;
        __pci_register_device(bus, slot, func);
      }
    }
  }
}

static int __pci_compare(struct pci_id ident1, struct pci_id * ident2)
{
  return (ident1.ven == ident2->ven && ident1.dev == ident2->dev);
}

static int __pci_driver_attach(struct pci_device * dev, struct pci_driver * drv)
{
  int rc;
  printm("Attaching driver \"%s\"...\n", drv->name);
  dev->driver = drv;
  rc = dev->driver->attach_driver(dev);
  return rc;
}

static void __pci_driver_init(struct pci_device * dev)
{
  dev->driver->init_driver();
}

static void __pci_drivers_find(void)
{
  int rc;
  for(int i = 0; i < pci_reg; i++)
  {
    struct pci_device d = devices[i];
    for(int j = 0; drivers[j]; j++)
    {
      int k = 0;
      struct pci_id * ident = &drivers[j]->ident[k];
      while(ident->ven != PCI_VENDOR_INVALID)
      {
        ident = &drivers[j]->ident[k++];
        if(__pci_compare(d.ident, ident))
        {
          rc = __pci_driver_attach(&d, drivers[j]);
          if(rc < 0)
          {
            printm("Driver attachment failed: %d\n", rc);
            continue;
          }
          printm("Initializing driver \"%s\"...\n", d.driver->name);
          __pci_driver_init(&d);
        }
      }
    }
  }
}

void pci_init(void)
{
  printm("Initializing PCI device subsystem...\n");
  __pci_enumerate();
  __pci_drivers_find();
}

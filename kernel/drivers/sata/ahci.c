#include <lunaris/pci.h>
#include <stdint.h>
#include <lunaris/printk.h>
#include <lunaris/module.h>

MODULE_NAME("sata")

static const struct pci_id sata_ids[] = 
{
  //PCI_ID(INTEL, 0x9dd3), // Cannon Point-LP SATA Controller [AHCI Mode]
  PCI_ID(0xffff, 0xffff) 
};

static int __ahci_driver_attach(struct pci_device * dev)
{
  printm("Fizz\n");
  return 0;
}

static void __ahci_driver_init(void)
{
  printm("Buzz\n");
}

struct pci_driver sata_driver =
{
  .name = "SATA Controller",
  .ident = (void *)&sata_ids,
  .attach_driver = __ahci_driver_attach,
  .init_driver = __ahci_driver_init
};

#ifndef __PCI_H
#define __PCI_H

#include <stdint.h>

// helper macros
#define PCI_ID(ven, dev) \
  {ven, dev}

#define INTEL 0x8086

struct pci_device;

struct pci_id
{
  uint16_t ven;
  uint16_t dev;
};

struct pci_driver
{
  char * name;
  struct pci_id * ident;
  int (*attach_driver)(struct pci_device *);
  void (*init_driver)(void);
};

struct pci_device
{
  struct pci_id ident;
  uint8_t prog_if;
  uint8_t subclass;
  uint8_t class;
  uint8_t bus, slot, functions;
  struct pci_driver * driver;
};

uint32_t pci_get_bar(uint8_t bus, uint8_t slot, uint8_t func, uint8_t bar);
void pci_init(void);

#endif

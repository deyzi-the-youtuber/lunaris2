#include <lunaris/device.h>
#include <stdint.h>
#include <common.h>
#include <lunaris/memory.h>
#include <lunaris/printk.h>
#include <lunaris/module.h>

MODULE_NAME("dev")

struct device_t * devices[32];

void devices_init(void)
{
  printm("Initialized device subsystem\n");
  memset(devices, 0, sizeof(devices));
}

void device_register(struct device_t * dev)
{
  // dummy function
  printm("device name: %s\n", dev->name);
}

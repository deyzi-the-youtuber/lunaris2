#include <lunaris/device.h>
#include <stdint.h>
#include <common.h>
#include <lunaris/memory.h>
#include <lunaris/printk.h>
#include <lunaris/module.h>
#include <errno.h>

MODULE_NAME("dev")

struct device_t * devices[32] = {NULL};
ssize_t dev = 0;

void devices_init(void)
{
  printm("Initializing device subsystem...\n");
  memset(devices, 0, sizeof(struct device_t) * 32);
}

void device_read(struct device_t * device, void * buf, ssize_t sz)
{
  if(!device || !device->ops->read)
    return;
  device->ops->read(buf, sz);
}

void device_write(struct device_t * device, void * buf, ssize_t sz)
{
  if(!device || !device->ops->write)
    return;
  device->ops->write(buf, sz);
}

int device_register(struct device_t * device)
{
  if(!device)
    return -EINVAL;
  printm("Registering device: \"%s\"\n", device->name);
  devices[dev++] = device;
  return 0;
}

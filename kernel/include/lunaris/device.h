#ifndef __DEVICE_H
#define __DEVICE_H

#include <sys/types.h>

struct dev_ops
{
  void (*read)(void *, ssize_t);
  void (*write)(void *, ssize_t);
};

struct device_t 
{
  char * name;
  dev_t num;
  struct dev_ops * ops;
  void * priv_data;
};

void devices_init(void);
void device_register(struct device_t * dev);

#endif

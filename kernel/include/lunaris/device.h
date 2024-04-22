#ifndef __DEVICE_H
#define __DEVICE_H

#include <sys/types.h>

#define DEVICE_UNKNOWN    0
#define DEVICE_TYPE_FILE  1
#define DEVICE_TYPE_CHAR  2
#define DEVICE_TYPE_BLOCK 3

struct device_ops
{
  void (*read)(void *, ssize_t);
  void (*write)(void *, ssize_t);
};

struct device_t 
{
  char * name;
  int device_type;
  struct device_ops * ops;
  void * priv_data;
};

void devices_init(void);
void device_read(struct device_t * device, void * buf, ssize_t sz);
void device_write(struct device_t * device, void * buf, ssize_t sz);
int device_register(struct device_t * device);

#endif

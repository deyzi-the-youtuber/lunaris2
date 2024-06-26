#ifndef __MEMORY_H
#define __MEMORY_H

#include <stdint.h>
#include <sys/types.h>

void kheap_init(uint32_t * start);
ssize_t kheap_get_used(void);
void * kmalloc(ssize_t size);
void kfree(void * ptr);

#endif

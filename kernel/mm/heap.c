#include <lunaris/memory.h>
#include <lunaris/printk.h>
#include <lunaris/module.h>
#include <stdint.h>
#include <common.h>

MODULE_NAME("Kheap");

struct kheap_metadata
{
  uint32_t size;
  bool used;
};

struct kheap_block
{
  //uint32_t magic;
  struct kheap_metadata metadata;
};

// The kernel heap can be increased up to 8 MB using kbrk.
// Much of the space after 0x500000 is reserved for this purpose.
#define KERNEL_HEAP_MAXIMUM 0x800000

uint32_t kheap_start = 0, kheap_end = 0, kheap_used = 0;
uint32_t prev_alloc = 0;

void kheap_init(uint32_t * start)
{
  kheap_start = (uint32_t)start;
  prev_alloc = kheap_start;
  kheap_end = 0x500000;
  printm("kernel heap starts at 0x%x\n", kheap_start);
  // clear all memory in the kernel heap range
  memset((void *)kheap_start, 0, KERNEL_HEAP_MAXIMUM - kheap_start);
}

ssize_t kheap_get_used(void)
{
  return kheap_used;
}

// just like brk or sbrk but with the kernel heap
static void * kbrk(ssize_t size)
{
  if(size < 1)
    return NULL;
  if(kheap_start + kheap_used >= KERNEL_HEAP_MAXIMUM)
    return NULL;
  void * ptr;
  kheap_end += size;
  ptr = (void *)kheap_end;
  return ptr;
}

static void * __heap_get_free(ssize_t size)
{
  struct kheap_block * block = NULL;
  uint32_t * mem = (uint32_t *)kheap_start;
  // if this is our first alloc, return the start of the kernel heap
  if(prev_alloc == kheap_start)
  {
    return (void *)prev_alloc;
  }
  while((uint32_t)mem < prev_alloc)
  {
    block = (struct kheap_block *)mem;
    // free blocks usually dont have a size
    if(!block->metadata.size)
    {
      return mem;
    }
    // block can be used even though the size is a bit larger
    // its just needs to be truncated
    if(block->metadata.size >= size && block->metadata.used == false)
    {
      return mem;
    }
    if(block->metadata.used)
    {
      //printm("Block info: Used: %s, Size: %d bytes\n", block->metadata.used ? "True" : "False", block->metadata.size);
      mem += block->metadata.size;
      mem += sizeof(struct kheap_block);
    }
    mem += size;
    mem += sizeof(struct kheap_block);
  }
  return mem;
}

// FINALLY THIS HEAP ALLOCATOR WORKS

void * kmalloc(ssize_t size)
{
  if(!size)
    return NULL;
  struct kheap_block * b = NULL;
  b = __heap_get_free(size);
  ssize_t usage = prev_alloc + size + sizeof(struct kheap_block);
  // we either 
  if(usage > kheap_end)
  {
    if(kheap_end >= KERNEL_HEAP_MAXIMUM)
    {
      return NULL;
    }
    if (!kbrk(size))
    {
      return NULL;
    }
  }

  // update variables
  
  prev_alloc += size;
  prev_alloc += sizeof(struct kheap_block);

  kheap_used += size;
  kheap_used += sizeof(struct kheap_block);
  // update header
  b->metadata.size = size;
  b->metadata.used = true;
  printm("Allocated %d bytes\n", b->metadata.size);
  memset((void *)(b + sizeof(struct kheap_block)), 0, size);
  return (void *)((uint32_t)b + sizeof(struct kheap_block));
}

void kfree(void * ptr)
{
  if(!ptr)
  {
    printm("WARNING: Freeing a null pointer.\n");
    return;
  }
  struct kheap_block * b = (struct kheap_block *)(ptr - sizeof(struct kheap_block));
  if(!b->metadata.used)
    return;
  ssize_t size = b->metadata.size;
  // update variables
  kheap_used -= b->metadata.size;
  kheap_used -= sizeof(struct kheap_block);
  b->metadata.used = false;
  printm("Freed %d bytes from heap\n", b->metadata.size);
}

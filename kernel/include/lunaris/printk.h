#ifndef __PRINTK_H
#define __PRINTK_H

#include <sys/types.h>

#define KERN_INFO "\033[39m"
#define KERN_WARN "\033[33m"
#define KERN_CRIT "\033[91m"
#define KERN_ERR  "\033[31m"

ssize_t printk(const char fmt[], ...);
void panic(const char fmt[], ...);

#endif

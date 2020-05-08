#ifndef KERNELPATCHER_H
#define KERNELPATCHER_H

#include <stdint.h>
#include <stdint.h>

#ifndef dprintf
int printf_(const char* format, ...); // XXX : This sucks.
#define dprintf printf_
#endif

#undef printf
#define printf printf_

void* find_kernel_entry(char* kernel);
int patch_kernel(char* kernel, uint32_t kernel_virt_base, uint32_t kernel_phys_base, char* boot_args);

#endif
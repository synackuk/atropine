#ifndef PATCHERS_H
#define PATCHERS_H

#include <stdlib.h>
#include <stdint.h>
#include "includes/instructions.h"

char* ultoa(unsigned long val, char* s, int radix);

int patch_mapforio(uintptr_t phys_base);
int patch_tfp0(char* address, uintptr_t phys_base, uintptr_t virt_base);
int patch_amfi(char* address, uintptr_t phys_base, uintptr_t virt_base);
int patch_mount(char* address, uintptr_t phys_base, uintptr_t virt_base, char* boot_args);
int patch_i_can_has_debugger(char* address, uintptr_t phys_base, uintptr_t virt_base);
int patch_platform_binary(char* address, uintptr_t phys_base, uintptr_t virt_base);
int patch_entitlements(char* address, uintptr_t phys_base, uintptr_t virt_base);
int patch_sandbox(char* address, uintptr_t phys_base, uintptr_t virt_base);

#endif
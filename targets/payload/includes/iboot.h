#ifndef IBOOT_H
#define IBOOT_H

#include <xnu.h>

#define IBOOT_MAGIC 0xEA00000E

int iboot_init(boot_args* args);
void iboot_boot(int should_patch);

#endif
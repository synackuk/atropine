#ifndef FINDERS_H
#define FINDERS_H

#include <plib.h>
#include <constants.h>

#define MASK(x, y, z) (((x) >> (y)) & ((1 << (z)) - 1))

uintptr_t* find_base_address();
uintptr_t* find_load_address();
uintptr_t* find_cmd_ptr();
malloc_t find_malloc();
get_env_t find_get_env();
get_env_uint_t find_get_env_uint();
uintptr_t* find_framebuffer_address();
uint32_t find_display_width();
uint32_t find_display_height();
void* find_image_list();
int find_version();
set_env_uint_t find_set_env_uint();

#endif
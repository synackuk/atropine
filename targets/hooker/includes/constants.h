#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <plib.h>
#include <command.h>

extern uintptr_t* base_address;
extern uintptr_t* load_address;
extern uintptr_t* framebuffer_address;
extern uint32_t display_width;
extern uint32_t display_height;
extern command_descriptor* cmd_ptr_address;
extern void* image_list;
extern int version;

typedef void* (*malloc_t)(size_t size);
extern malloc_t _malloc;
#define malloc _malloc

typedef char* (*get_env_t)(char* env);
extern get_env_t _get_env;
#define get_env _get_env

typedef uintptr_t (*get_env_uint_t)(char* env);
extern get_env_uint_t _get_env_uint;
#define get_env_uint _get_env_uint

typedef int (*set_env_uint_t)(char* env, uint32_t val, int unk);
extern set_env_uint_t _set_env_uint;
#define set_env_uint _set_env_uint

int constants_init();

#endif
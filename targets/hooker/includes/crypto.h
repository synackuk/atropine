#ifndef CRYPTO_H
#define CRYPTO_H

#include <image.h>


#define AES_KBAG_SIZE 0x30

#define AES_DECRYPT 0x11

#define AES_TYPE_GID 0x20000200
#define AES_TYPE_256 0x20000000

#define IMAGE_DATA 0x44415441
#define IMAGE_KBAG 0x4B424147

int decrypt_kbag(void* kbag);
int decrypt_data(void* data, size_t len, image_kbag* kbag);

#endif
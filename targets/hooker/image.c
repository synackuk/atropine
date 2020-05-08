#include <plib.h>
#include <constants.h>
#include <image.h>
#include <crypto.h>

static void* image_find_tag(void* image, unsigned int tag, unsigned int size) {
	unsigned int i = 0;
	unsigned int* current = image;
	current = image;
	for (i = 0; i < size; i++) {
		if (*current == tag) {
			return current;
		}
		current++;
	}
	return 0;
}

int bdev_read(void *bdev, void *buf, long long offset, long long size)
{
	if (version < 1940) {
		return ((struct bdev6_t *)bdev)->bdev_read(bdev, buf, offset, size);
	}
	return ((struct bdev_t *)bdev)->bdev_read(bdev, buf, offset, size);
}

int load_image_from_bdev(char* address, uint32_t tag, size_t* len) {
	uint32_t ret;
	struct firmware_image* image = ((firmware_image*)image_list)->next;
	while(image != image_list) {
		if(image->info.type == tag) {
			break;
		}
		image = image->next;
	}

	if(image == image_list) {
		return -1;
	}

	ret = bdev_read(image->bdev, address, image->offset_lo, image->info.size);
	if(ret != image->info.size) {
		return -1;
	}
	*len = image->info.size;
	return 0;
}

int load_image(char* address) {
	int ret;
	img3_root* header = (img3_root*) address;
	size_t len = header->tag.size;
	img3_tag* data_head = (img3_tag*)image_find_tag(address, IMAGE_DATA, len);
	if(!data_head) {
		return -1;
	}
	void* data = (void*)((uintptr_t)data_head + sizeof(img3_tag));
	image_kbag* kbag = (image_kbag*)image_find_tag(address, IMAGE_KBAG, len);
	if(!kbag) {
		return -1;
	}
	ret = decrypt_kbag(kbag->iv);
	if(ret != 0) {
		return -1;
	}
	ret = decrypt_data(data, (data_head->data_size - (data_head->data_size % 16)), kbag);
	if(ret != 0) {
	}
	memcpy(address, data, (data_head->data_size - (data_head->data_size % 16)));
	return 0;
}














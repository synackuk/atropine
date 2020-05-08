#include <constants.h>
#include <crypto.h>

int decrypt_kbag(void* kbag) {
	int ret;
	ret = aes_crypto_cmd(AES_DECRYPT, kbag, kbag, AES_KBAG_SIZE, AES_TYPE_GID, 0, 0);
	if(ret != 0) {
		return -1;
	}
	return 0;
}

int decrypt_data(void* data, size_t len, image_kbag* kbag) {
	int ret;
	ret = aes_crypto_cmd(AES_DECRYPT, data, data, len, AES_TYPE_256, (char*)kbag->key, (char*)kbag->iv);
	if(ret != 0) {
		return -1;
	}
	return 0;
}
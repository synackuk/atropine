#include <plib.h>

void panic(char* err, ...) {
	printf("Panic!\n");
	va_list va;
	va_start(va, err);
	vprintf(err, va);
	va_end(va);
	printf("Hanging here.\n");
	while(1) {}
}
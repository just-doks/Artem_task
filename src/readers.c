#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "my_header.h"

unsigned long readBytesAsInt(FILE *file, size_t size, unsigned long address) {
	unsigned long variable = 0;
	fseek(file, address, SEEK_SET);
	fread(&variable, size, 1, file);
	return variable;
}

char * readBytesAsString(FILE *file, size_t size, unsigned long address) {
	char *string = (char*) calloc(size + 1, sizeof(char));
	char l;
	int i = 0;
	fseek(file, address, SEEK_SET);
	for (i = 0; i < size; i++) {
		fread(&l, 1, 1, file);
		*(string + i) = ((l >= 0x20) && (l <= 0x7E)) ? l : '_';
	}
	*(string + i) = '\0';
	return string;
}

unsigned short * readBytesAsUShort(FILE *file, unsigned amount, unsigned long address) {
		unsigned short * array = (unsigned short *) calloc(amount / 2, sizeof(unsigned short));
		fseek(file, address, SEEK_SET);
		fread(array, sizeof(unsigned short), amount / 2, file);
		return array;
}
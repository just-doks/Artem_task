#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "my_header.h"


int main (int argc, char *argv[]) {
	// 1
	if (argc < 2) return 0;
	char * string;
	string = *(argv + 1);
	printf("%s\n", string);

	FILE *file;
	printf("Connecting... ");
	if (!(file = fopen(string, "rb+"))) {
		perror("Invalid path\n");
		exit(1);
	} else {
		printf("Success\n");
	}

	// 2
	BPBSector bpb = getBPB(file);
	if (checkBPB(bpb) != 0) {
		printf("wrong path\n");
		return 0;
	}
	printBPB(bpb);

	// 3
	Drive drive = getDrive(file, bpb);
	printDrive(drive);

	// 4
	Array array = getFiles(drive);
	DriveFile *list = array.ptr;
	unsigned list_size = array.size;

	// 5
	Table *table = getTable(drive);
	table = markTable(drive, table, list, list_size);

	// 6
	defragmentFreeSpace(drive, table, list, list_size);
	printf("/////////////////////////////////////\n");

	// 7
	printf("Fragments before defragmentation: %d\n", checkFreeSpace(drive));
	free(drive.fat_table);
	drive.fat_table = readBytesAsUShort(file, drive.fat_size, drive.fat_address);
	printf("Fragments after defragmentation: %d\n", checkFreeSpace(drive));
	
	free(drive.fat_table);
	free(table);
	free(list);
	fclose(file);

	return 0;
}
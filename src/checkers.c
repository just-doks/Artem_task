#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "my_header.h"

int checkBPB(BPBSector bpb) {
		if (bpb.sectorsInFAT != 0
		&& bpb.sizeOfRootDir != 0
		&& (bpb.bytesInSector == 512 
			|| bpb.bytesInSector == 1024
			|| bpb.bytesInSector == 2048
			|| bpb.bytesInSector == 4096)
		&& (bpb.sectorsInCluster == 2
			|| bpb.sectorsInCluster == 4
			|| bpb.sectorsInCluster == 8
			|| bpb.sectorsInCluster == 16
			|| bpb.sectorsInCluster == 32
			|| bpb.sectorsInCluster == 64)) 
	{
		return 0;
	} else {
		return -1;
	}
}

unsigned checkFreeSpace(Drive drive) {
	unsigned i = 0;
	unsigned fragments = 0;
	short flag = 0;
	for (i = 3; i < drive.fat_size / 2; i++) {

		if ((*(drive.fat_table + i) == 0)) {
			flag = 1;
			continue;
		}

		if ((*(drive.fat_table + i) != 0) && (flag == 1)) {
			fragments += 1;
			flag = 0;
		}
	}
	if (flag == 1) fragments += 1;

	return fragments;
}
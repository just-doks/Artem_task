#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "my_header.h"

void defragmentFreeSpace(Drive drive, Table *table, DriveFile *list, unsigned list_size) {
	unsigned i = 0;
	unsigned short new_cell = 0;
	unsigned zeros = 0;
	// 6.1
	for (i = 3; i < drive.fat_size / 2; i++) {
		if (i < 3) {
			perror("Unpredicted behavior. i < 3\n");
			exit(3);
		}

		if ((table + i)->occupied == 0) {
			zeros++;
			continue;
		}

		if (((table + i)->occupied != 0) && (zeros != 0)) {
			DriveFile dfile = *(list + (table + i)->occupied - 1);

			/////////////////////////////////////////////////////////
			new_cell = i - zeros;
			/////////////////////////////////////////////////////////

			if ((table + i)->first != 0) {
				// 6.2
				unsigned short next = 0;
				fseek(drive.file, drive.fat_address + dfile.first_cluster * 2, SEEK_SET);
				fread(&next, 2, 1, drive.file);

				/////////////////////////////////////////////////////////////////////////
				moveFirstCluster(drive, *(list + (table + i)->occupied - 1), new_cell);
				(list + (table + i)->occupied - 1)->first_cluster = new_cell;
				/////////////////////////////////////////////////////////////////////////

				if ((next != 0xFFFF) || (next != 0)) {
					(table + next)->prev = new_cell;
				}
			} else {
				// 6.3
				moveCluster(drive, (table + i)->prev, new_cell);
				(table + new_cell)->prev = (table + i)->prev;
				(table + i)->prev = 0;
			}

			(table + new_cell)->occupied = (table + i)->occupied;
			(table + i)->occupied = 0;
		
			(table + new_cell)->first = (table + i)->first;
			(table + i)->first = 0;

			i -= zeros;
			zeros = 0;

		} // if()

	} // for()

} //defragmentFreeSpace()
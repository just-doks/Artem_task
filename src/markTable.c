#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "my_header.h"


Table * markTable(Drive drive, Table *table, DriveFile *list, unsigned list_size) {
	unsigned i = 0;
	unsigned short current = 0, next = 0;

	for (i = 0; i < list_size; i++) {
		(table + (list + i)->first_cluster)->occupied = i + 1;
		(table + (list + i)->first_cluster)->first = i + 1;

		current = (list + i)->first_cluster;
		next = *(drive.fat_table + current);

		while(next != 0xFFFF) {

			if (next == 0) {
				perror("markTable: unpredicted behavior\n");
				exit(2);
			}

			(table + next)->occupied = i + 1;
			(table + next)->prev = current;
			current = next;
			next = *(drive.fat_table + current);
		}
	}
	return table;
}
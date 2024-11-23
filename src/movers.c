#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "my_header.h"

void moveCluster(Drive drive, unsigned short prev_cell, unsigned short nw_cell) {
	unsigned char *cluster = (unsigned char*) calloc(drive.sizeOfCluster, sizeof(unsigned char));
	unsigned long offset = 0;
	unsigned short new_cell = 0, current_cell = 0, previous_cell = 0, next_cell = 0, zero = 0;
	unsigned i = 0;

	previous_cell = prev_cell; // должно быть указано
	fseek(drive.file, drive.fat_address + previous_cell * 2, SEEK_SET);
	fread(&current_cell, 2, 1, drive.file);
	printf("MC currect cluster = %04X\n", current_cell);

	new_cell = nw_cell; // должно быть указано

	fseek(drive.file, drive.fat_address + current_cell * 2, SEEK_SET);
	fread(&next_cell, 2, 1, drive.file);

	printf("previous_cell = %d\n", previous_cell);
	printf("current_cell = %d\n", current_cell);
	printf("new_cell = %d\n", new_cell);
	printf("next_cell = %d\n", next_cell);

	// перемещение кластера в области данных
	// по смещению читается кластер, который будет перемещён
	offset = drive.dataSpace_address + ((current_cell - 1) * drive.sizeOfCluster);
	fseek(drive.file, offset, SEEK_SET);
	fread(cluster, 1, drive.sizeOfCluster, drive.file);

	// данные записываются в новый указанный кластер
	offset = drive.dataSpace_address + ((new_cell - 1) * drive.sizeOfCluster);
	fseek(drive.file, offset, SEEK_SET);
	fwrite(cluster, 1, drive.sizeOfCluster, drive.file);
	fflush(drive.file);


	free(cluster);

	// перезапись таблиц FAT
	for (i = 0; i < drive.amountOfFAT; i++) {

		// ячейка в фат таблице, указывающая на перемещённый кластер, перезаписывается 
		offset = drive.fat_address + (previous_cell * 2) + (drive.fat_size * i);
		fseek(drive.file, offset, SEEK_SET);
		fwrite(&new_cell, sizeof(unsigned short), 1, drive.file);

		// номер следующего в цепочке кластера записывается в новую ячейку
		offset = drive.fat_address + (new_cell * 2) + (drive.fat_size * i);
		fseek(drive.file, offset, SEEK_SET);
		fwrite(&next_cell, sizeof(unsigned short), 1, drive.file);

		// зануление старой ячейки в таблице FAT
		offset = drive.fat_address + (current_cell * 2) + (drive.fat_size * i);
		//printf("offset: %08X\n", offset);
		fseek(drive.file, offset, SEEK_SET);
		fwrite(&zero, sizeof(unsigned short), 1, drive.file);
	}
}

void moveFirstCluster(Drive drive, DriveFile dfile, unsigned short new_cell) {

	if (dfile.first_cluster == new_cell) return;

	unsigned char *cluster = (unsigned char*) calloc(drive.sizeOfCluster, sizeof(unsigned char));
	unsigned long address = 0;
	unsigned short next_cell = 0, zero = 0;
	unsigned i = 0;

	fseek(drive.file, drive.fat_address + dfile.first_cluster * 2, SEEK_SET);
	fread(&next_cell, 2, 1, drive.file);

	// перемещение кластера в области данных
	// по смещению читается кластер, который будет перемещён
	address = drive.dataSpace_address + (dfile.first_cluster - 1) * drive.sizeOfCluster;
	fseek(drive.file, address, SEEK_SET);
	fread(cluster, 1, drive.sizeOfCluster, drive.file);

	// данные записываются в новый указанный кластер
	address = drive.dataSpace_address + (new_cell - 1) * drive.sizeOfCluster;
	fseek(drive.file, address, SEEK_SET);
	fwrite(cluster, 1, drive.sizeOfCluster, drive.file);
	fflush(drive.file);

	// перезапись первого кластера в 32 байтовой записи файла в корневой директории
	fseek(drive.file, dfile.address + 0x1A, SEEK_SET);
	fwrite(&new_cell, 2, 1, drive.file);
	fflush(drive.file);


	free(cluster);

	// перезапись таблиц FAT
	for (i = 0; i < drive.amountOfFAT; i++) {
		// номер следующего в цепочке кластера (или 0xFFFF) записывается в новую ячейку
		address = drive.fat_address + (new_cell * 2) + (drive.fat_size * i);
		fseek(drive.file, address, SEEK_SET);
		fwrite(&next_cell, sizeof(unsigned short), 1, drive.file);
		fflush(drive.file);

		// зануление старой ячейки в таблице FAT
		address = drive.fat_address + (dfile.first_cluster * 2) + (drive.fat_size * i);
		fseek(drive.file, address, SEEK_SET);
		fwrite(&zero, sizeof(unsigned short), 1, drive.file);
		fflush(drive.file);
	}
}
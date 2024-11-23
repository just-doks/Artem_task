#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "my_header.h"

void printBPB(BPBSector bpb) {
	printf("======================================================\n");
	printf("Bytes in one sector: %d\n", bpb.bytesInSector);
	printf("Sectors in one cluster: %d\n", bpb.sectorsInCluster);
	printf("Amount of sectors in BIOS Parameter Block: %d\n", bpb.sectorsInBPB);
	printf("Amount of FAT tables: %d\n", bpb.amountOfFAT);
	printf("Size of Root Directory: %d bytes / %d records\n", bpb.sizeOfRootDir, bpb.sizeOfRootDir/32);
	printf("Sectors in one FAT table: %d\n", bpb.sectorsInFAT);
	printf("Size of Drive: %d Bytes / %.3lf Gb\n", bpb.sizeOfDrive, (double) bpb.sizeOfDrive / (1024*1024*1024));
	printf("======================================================\n");
}

void printDrive(Drive drive) {
	printf("======================================================\n");
	char * fat_type = readBytesAsString(drive.file, 8, 0x36);
	char *drive_name = readBytesAsString(drive.file, 11, 0x2B);
	printf("Drive name is: %s\n", drive_name);
	printf("FAT type is: %s\n", fat_type);
	printf("fat size: %d Bytes\n", drive.fat_size);
	printf("Amount of address units in fat table: %d\n", drive.fat_size / 2);
	printf("Size of cluster: %d Bytes\n", drive.sizeOfCluster);
	printf("Clusters amount (last cluster): %d\n", drive.lastCluster);
	unsigned long av_space = drive.sizeOfCluster * drive.lastCluster;
	printf("Available space for data: %d Bytes or %.3lf Gb\n", av_space, (double) av_space / (1024 * 1024 * 1024));
	printf("FAT address: %08X hex\n", drive.fat_address);
	printf("Data Space address: %08X hex\n", drive.dataSpace_address);
	printf("======================================================\n");
	free(fat_type);
	free(drive_name);
}

void printFilesList(DriveFile *list, unsigned size) {
	unsigned i = 0;
	for (i = 0; i < size; i++) {
		printf("File %d. Record address: %08X / first cluster: %04X\n", 
			i + 1, (list + i)->address, (list + i)->first_cluster);
	}
}

// функция отображения данных секторами, по смещению - (file, amount of sectors, offset)
void printSectors(FILE *file, unsigned short sectors, unsigned long int offset) {

    unsigned char a = 0; // буфер для байта
    int i = 0, j = 0, k = 0; // счётчики
    fseek(file, offset, SEEK_SET); // возвращаем указатель в файле на начало
    // здесь реализован красивый вывод
    for (i = 0; i < sectors * 0x20; i++) { // смещение по оси Y

        if ((i % 512) == 0) {
            for (j = 0; j < 16; j++) {
                if (j == 0) printf("\t\t");
                printf("x%01X ", j);
                if (j == 7) printf("    ");
                if (j == 0xF) printf("\n\n"); 
            }
        }
        
    	//printf("%d.\t", ++k);
        printf("%08X\t", offset + 0x10 * i);
    
        for (j = 0; j < 0x10; j++) { // смещение по оси X
    		fread(&a, 1, 1, file); // Читаем байты из файла
            if (a <= 0xF) { // Рисуем нолик для красоты, если байт меньше 16
                printf ("0%X ", a);
            } else {
                printf("%X ", a);
            }
            if (j == 7) printf("%s", "    "); // пропуск после каждого восьмого байта
        }

    	printf("%s", "\n"); // переход на новую строку каждые 16 байт
    	//if (k % 2 == 0) printf("\n");

        if ((i+1) % 0x20 == 0) printf("%s", "\n"); // делаем пропуск после конца каждого сектора
    }
    fseek(file, 0, SEEK_SET); // возвращаем указатель в файле на начало
}

void printTable(Table *table, unsigned amount) {
	unsigned i = 0;
	for (i = 0; i < amount; i++) {
		printf("FAT_N %d. occupied: %d / first: %d / prev: %04X\n", i, (table + i)->occupied, (table + i)->first, (table + i)->prev);
	}
}
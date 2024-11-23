#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "my_header.h"

BPBSector getBPB(FILE *file) {
	BPBSector bpb;

	bpb.bytesInSector = readBytesAsInt(file, 2, 0x0B);

	bpb.sectorsInCluster = readBytesAsInt(file, 1, 0x0D);

	bpb.sectorsInBPB = readBytesAsInt(file, 2, 0x0E);

	bpb.amountOfFAT = readBytesAsInt(file, 1, 0x10);

	bpb.sizeOfRootDir = readBytesAsInt(file, 2, 0x11);

	bpb.sectorsInFAT = readBytesAsInt(file, 2, 0x16);

	bpb.sizeOfDrive = readBytesAsInt(file, 4, 0x20) * bpb.bytesInSector;
	return bpb;
}


Drive getDrive(FILE *file, BPBSector bpb) {

	Drive drive;

	drive.fat_size = bpb.sectorsInFAT * bpb.bytesInSector;

	drive.amountOfFAT = bpb.amountOfFAT;

	drive.fat_address = bpb.bytesInSector * bpb.sectorsInBPB;

	drive.dataSpace_address = bpb.bytesInSector * (bpb.sectorsInBPB
		+ bpb.sectorsInFAT * bpb.amountOfFAT);

	drive.sizeOfCluster = bpb.bytesInSector * bpb.sectorsInCluster;

	drive.lastCluster = (bpb.sizeOfDrive - drive.dataSpace_address)
		/ drive.sizeOfCluster;


	drive.fat_table = readBytesAsUShort(file, drive.fat_size, drive.fat_address);

	drive.file = file;

	return drive; 
}

Array getFiles(Drive drive) {
	unsigned i = 0, size = 0;
	unsigned long var = 0, address = 0;
	unsigned short attr = 0, frstCl = 0;

	for (i = 0; i < drive.sizeOfCluster; i += 0x20) {
		address = drive.dataSpace_address + i;
		// читаем первый байт 32 байтовой записи файла
		fseek(drive.file, address, SEEK_SET);
		fread(&var, 1, 1, drive.file);
		// если байт равен нулю - дальше записей нет, завершаем цикл
		if (var == 0) break;

		//если у файла метка удалённого файла - пропустить
		if (var == 0xE5 || var == 0x05) continue;

		// читаем атрибут файла
		fseek(drive.file, address + 0x0B, SEEK_SET);
		fread(&attr, 1, 1, drive.file);
		// если атрибут файла не соответствует атрибуту архивного файла - пропустить
		if ((attr & 0x20) != 0x20) continue;

		// читаем первый кластер файла
		fseek(drive.file, address + 0x1A, SEEK_SET);
		fread(&frstCl, 2, 1, drive.file);
		// если номер первого кластера не указан - пропускаем
		if (frstCl == 0) continue;

		// main block //////////////////////////////////////////////////////////
		size++;
		////////////////////////////////////////////////////////// main block //
	}

	DriveFile *list = (DriveFile*)calloc(size, sizeof(DriveFile));
	unsigned short flag = 0;

	for (i = 0; i < drive.sizeOfCluster; i += 0x20) {
		address = drive.dataSpace_address + i;
		// читаем первый байт 32 байтовой записи файла
		fseek(drive.file, address, SEEK_SET);
		fread(&var, 1, 1, drive.file);
		// если байт равен нулю - дальше записей нет, завершаем цикл
		if (var == 0) break;

		//если у файла метка удалённого файла - пропустить
		if (var == 0xE5 || var == 0x05) continue;

		// читаем атрибут файла
		fseek(drive.file, address + 0x0B, SEEK_SET);
		fread(&attr, 1, 1, drive.file);
		// если атрибут файла не соответствует атрибуту архивного файла - пропустить
		if ((attr & 0x20) != 0x20) continue;

		// читаем первый кластер файла
		fseek(drive.file, address + 0x1A, SEEK_SET);
		fread(&frstCl, 2, 1, drive.file);
		// если номер первого кластера не указан - пропускаем
		if (frstCl == 0) continue;

		// main block //////////////////////////////////////////////////////////
		// создаём экзмпляр структуры, хранящей первый кластер файла и адрес
		// его записи в корневой директории
		DriveFile dfile;
		dfile.address = address;
		dfile.first_cluster = frstCl;

		// добавляем экземпляр к списку
		*(list + flag++) = dfile;
		if (flag == size) break;
		////////////////////////////////////////////////////////// main block //
	}

	Array array;
	array.ptr = list;
	array.size = size;

	return array;

}

Table * getTable(Drive drive) {

	unsigned i = 0;
	Table *table = (Table*) calloc(drive.fat_size / 2, sizeof(Table));

	for (i = 0; i < drive.fat_size / 2; i++) {
		(table + i)->occupied = 0;
		(table + i)->first = 0;
		(table + i)->prev = 0;
	}

	return table;
}
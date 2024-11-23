#ifndef MY_HEADER_H
#define MY_HEADER_H

// Структура с данными из загрузочного сектора ФС ФАТ16
typedef struct bpbSector {
			// Число байт в секторе
			// Допустимые значения - 512, 1024, 2048 и 4096
			unsigned short bytesInSector; // address - 0x0B // size - 2 bytes

			// Количество секторов на кластер
			// допустимо 2, 4, 8, 16, 32 (также 64, поддерживается не во всех системах)
			unsigned short sectorsInCluster; // address - 0x0D // size - 1 byte

			// Количество зарезервированных секторов под загрузочный раздел
			unsigned short sectorsInBPB; // address - 0x0E // size - 2 bytes

			// количество таблиц ФАТ
			unsigned short amountOfFAT; // address - 0x10 // size - 1 byte

			// Размер корневой директории
	    	unsigned short sizeOfRootDir; // address - 0x11 // size - 2 bytes

			// Количество секторов в одной таблице ФАТ
			// равно нулю, если это FAT32
			unsigned short sectorsInFAT; // address - 0x16 // size - 2 bytes

			// размер накопителя, указывается в секторах
			unsigned sizeOfDrive; // адрес - 0x20 // size - 4 bytes

} BPBSector;

typedef struct driveData {
	//BPBSector bpb;

	unsigned short * fat_table;
	unsigned long fat_size;
	unsigned short amountOfFAT;

	unsigned long fat_address;
	unsigned long dataSpace_address;
	
	unsigned sizeOfCluster;
	unsigned long lastCluster;

	FILE *file;

} Drive;

typedef struct array {
	void *ptr;
	unsigned size;
} Array;

typedef struct driveFile {
	unsigned long address;
	unsigned short first_cluster;
} DriveFile;

typedef struct table {
	unsigned long occupied;
	unsigned long first;
	unsigned short prev;
} Table;


// READERS.C /////////////////////////////////////////////////////////////////////////////
	unsigned long readBytesAsInt(FILE *file, size_t size, unsigned long address);

	char * readBytesAsString(FILE *file, size_t size, unsigned long address);

	unsigned short * readBytesAsUShort(FILE *file, unsigned amount, unsigned long address);
///////////////////////////////////////////////////////////////////////////// READERS.C //


// GETTERS.C /////////////////////////////////////////////////////////////////////////////
	BPBSector getBPB(FILE *file);

	Drive getDrive(FILE *file, BPBSector bpb);

	Array getFiles(Drive drive);

	Table * getTable(Drive drive);
///////////////////////////////////////////////////////////////////////////// GETTERS.C //


// PRINTERS.C ////////////////////////////////////////////////////////////////////////////
	void printBPB(BPBSector bpb);

	void printDrive(Drive drive);

	void printFilesList(DriveFile *list, unsigned size);

	void printSectors(FILE *file, unsigned short sectors, unsigned long int offset);

	void printTable(Table *table, unsigned amount);

//////////////////////////////////////////////////////////////////////////// PRINTERS.C //


// CHECKERS.C ////////////////////////////////////////////////////////////////////////////
	int checkBPB(BPBSector bpb);

	unsigned checkFreeSpace(Drive drive);
//////////////////////////////////////////////////////////////////////////// CHECKERS.C //


// MOVERS.C ////////////////////////////////////////////////////////////////////////////
	void moveCluster(Drive drive, unsigned short prev_cell, unsigned short nw_cell);

	void moveFirstCluster(Drive drive, DriveFile dfile, unsigned short new_cell);
//////////////////////////////////////////////////////////////////////////// MOVERS.C //


// MARKTABLE.C ////////////////////////////////////////////////////////////////////////////
	Table * markTable(Drive drive, Table *table, DriveFile *list, unsigned list_size);
//////////////////////////////////////////////////////////////////////////// MARKTABLE.C //


// DEFRAGFREESPACE.C /////////////////////////////////////////////////////////////////////////
	void defragmentFreeSpace(Drive drive, Table *table, DriveFile *list, unsigned list_size);
///////////////////////////////////////////////////////////////////////// DEFRAGFREESPACE.C //

#endif /* MY_HEADER_H */
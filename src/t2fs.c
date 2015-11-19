/*
########################################################################
#
#		INF01142 - Operating Systems
#			Assignment 2 - File Systems
#
#			T2FS
#
#		João Lauro Garibaldi Jr		195505
#		Khin Baptista				217443
#
#
########################################################################
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "apidisk.h"
#include "t2fs.h"

struct t2fs_superbloco superblock;
int initialized = 0;

int identify2(char* name, int size){
	char id[] = "JoaoLauro_KhinBaptista\0";

	size = 23;
	memcpy(name, id, size);
	puts(name);

	t2fs_init();

	return 0;
}

int intFromWord(WORD data){
	int most	= (data ^ 0x00FF) << 1;
	int least	= (data ^ 0xFF00) >> 1;

	return most + least;
}

int intFromDWord(DWORD data){
	int most		= (data ^ 0x000000FF) << 3;
	int midmost		= (data ^ 0x0000FF00) << 1;
	int midleast	= (data ^ 0x00FF0000) >> 1;
	int least		= (data ^ 0xFF000000) >> 3;

	return most + midmost + midleast + least;
}

void t2fs_init(){
	if (initialized == 0){
		t2fs_readSuperBlock();
	}
}

void t2fs_readSuperBlock(){
	BYTE buffer[SECTOR_SIZE];
	read_sector(0, (char *)buffer);

	memcpy(superblock.Id,				buffer,			4);
	memcpy(superblock.Version,			buffer + 4,		2);
	memcpy(superblock.SuperBlockSize,	buffer + 6,		2);
	memcpy(superblock.DiskSize,			buffer + 8,		4);
	memcpy(superblock.NofSectors,		buffer + 12,	4);
	memcpy(superblock.SectorPerCluster,	buffer + 16,	4);
	memcpy(superblock.pFATSectorStart,	buffer + 20,	4);
	memcpy(superblock.sFATSectorStart,	buffer + 24,	4);
	memcpy(superblock.RootSectorStart,	buffer + 28,	4);
	memcpy(superblock.DataSectorStart,	buffer + 32,	4);
	memcpy(superblock.NofDirEntries,	buffer + 36,	4);

	initialized = 1;
}

int delete2 (char *filename){
	t2fs_init();
	return 0;
}

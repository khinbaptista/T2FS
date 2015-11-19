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

// Data kept in memory
struct t2fs_superbloco superblock;
int open_files[20] = { -1 };
int initialized = 0;

char* workingdir = "/";

// Helper functions
unsigned int intFromWord(WORD data);
unsigned int intFromDWord(DWORD data);
void t2fs_init();

// Functions
int identify2(char* name, int size){
	char *id = "JoaoLauro195505_KhinBaptista217443\0";	//35

	if (size > 35)
		memcpy(name, id, 35);
	else
		memcpy(name, id, size);

	puts(name);

	t2fs_init();

	return 0;
}

unsigned int intFromWord(WORD data){
	unsigned int most	= (data ^ 0x00FF) << 1;
	unsigned int least	= (data ^ 0xFF00) >> 1;

	return most + least;
}

unsigned int intFromDWord(DWORD data){
	unsigned int most		= (data ^ 0x000000FF) << 3;
	unsigned int midmost	= (data ^ 0x0000FF00) << 1;
	unsigned int midleast	= (data ^ 0x00FF0000) >> 1;
	unsigned int least		= (data ^ 0xFF000000) >> 3;

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

	printf("Superblock size: %d (no conversion) | %d (with conversion)\n\n",
		superblock.SuperBlockSize, intFromWord(superblock.SuperBlockSize));
}

FILE2 create2(char *filename){
	t2fs_init();
	return 0;
}

int delete2 (char *filename){
	t2fs_init();
	return 0;
}

FILE2 open2(char *filename){
	t2fs_init();
	return 0;
}

int close2(FILE2 handle){
	t2fs_init();
	return 0;
}

int read2(FILE2 handle, char *buffer, int size){
	t2fs_init();
	return 0;
}

int write2(FILE2 handle, char *buffer, int size){
	t2fs_init();
	return 0;
}

int seek2(FILE2 handler, unsigned int offset){
	t2fs_init();
	return 0;
}

int mkdir2(char *pathname){
	t2fs_init();
	return 0;
}

int rmdir2(char *pathname){
	t2fs_init();
	return 0;
}

DIR2 opendir2(char *pathname){
	t2fs_init();
	return 0;
}

int readdir2(DIR2 handle, DIRENT2 *dentry){
	t2fs_init();
	return 0;
}

int closedir2(DIR2 handle){
	t2fs_init();
	return 0;
}

int chdir2(char *pathname){
	t2fs_init();
	return 0;
}

int getcwd2(char *pathname, int size){
	t2fs_init();
	return 0;
}

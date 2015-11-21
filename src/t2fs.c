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

// ########################################

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "apidisk.h"
#include "t2fs.h"
#include "helper.h"

// ########################################

#define MAX_OPEN_FILES 20

typedef struct t2fs_record Record;

// ########################################
/* Data kept in memory */

int initialized = 0;

struct t2fs_superbloco superblock;
BYTE* fat;

int clusterSize;
int clusterCount;
int fatSize;
int fatSectorCount;

// String of the current work directory
char* workdir;

// record of open files
Record* open_files[MAX_OPEN_FILES];

// one cluster open for each file
BYTE* open_clusters[MAX_OPEN_FILES];

// offset of file inside open clluster
int open_offsetcluster[MAX_OPEN_FILES];

// offset of open file relative to file start
int open_offset[MAX_OPEN_FILES];

// ########################################
/* Functions */

int identify2(char* name, int size){
	char id[] = "Joao Lauro 195505 | Khin Baptista 217443";
	int length = strlen(id);

	if (size >= length)
		memcpy(name, id, length);
	else
		memcpy(name, id, size);

	t2fs_init();
	return 0;
}

void t2fs_init(){
	if (initialized == 0){
		workdir = "/";
		t2fs_readSuperblock();
		t2fs_readFAT();

		initialized = 1;
	}
}

void t2fs_readSuperblock(){
	BYTE buffer[SECTOR_SIZE];
	read_sector(0, (char*)buffer);

	memcpy(&superblock.Id,					buffer,			4);
	memcpy(&superblock.Version,				buffer + 4,		2);
	memcpy(&superblock.SuperBlockSize,		buffer + 6,		2);
	memcpy(&superblock.DiskSize,			buffer + 8,		4);
	memcpy(&superblock.NofSectors,			buffer + 12,	4);
	memcpy(&superblock.SectorPerCluster,	buffer + 16,	4);
	memcpy(&superblock.pFATSectorStart,		buffer + 20,	4);
	memcpy(&superblock.sFATSectorStart,		buffer + 24,	4);
	memcpy(&superblock.RootSectorStart,		buffer + 28,	4);
	memcpy(&superblock.DataSectorStart,		buffer + 32,	4);
	memcpy(&superblock.NofDirEntries,		buffer + 36,	4);

	clusterSize		= SECTOR_SIZE * superblock.SectorPerCluster;

	clusterCount	=
		(superblock.DiskSize - superblock.DataSectorStart) / clusterSize;

	fatSize 		= clusterCount * 16;
	fatSectorCount	= fatSize / SECTOR_SIZE;

	if (fatSize % SECTOR_SIZE != 0) fatSectorCount++;
}

void t2fs_ReadFAT(){
	int it;
	BYTE buffer[SECTOR_SIZE];

	fat = malloc(fatSize);

	for(it = 0; it < fatSectorCount; it++){
		read_sector(superblock.pFATSectorStart + it, (char*)buffer);
		memcpy(fat + it * SECTOR_SIZE, buffer, SECTOR_SIZE);
	}
}

WORD FAT(int cluster){
	return fat[(cluster - 2) * 16];
}

int DirExists(char *pathname){
	char *path = AbsolutePath(pathname);
	char *step;

	BYTE data[SECTOR_SIZE];
	Record buffer;

	if (path == NULL)
		return 0;

	if  (strlen(path) == 1 && path[0] == '/')
		return 1;

	step = strtok(path, "/");

	while (step != NULL){
		read_sector(superblock.RootSectorStart, (char *)data);
		memcpy(&buffer, data, sizeof(Record));

		// incomplete
	}

	return 0;
}

int FileExists(char *pathname){
	return 0;
}

char* AbsolutePath(char *pathname){
	int wdirlength = strlen(workdir);
	int pathlength = strlen(pathname);
	int it;
	char *buffer = malloc(pathlength);
	char *token;
	char *absolute = malloc(wdirlength + pathlength);

	if (pathname[0] != '.' && pathname[0] != '/')
		return NULL;

	if (pathname[0] == '/')
		absolute[0] = '/';
	else
		memcpy(absolute, workdir, wdirlength);

	memcpy(buffer, pathname, pathlength);

	token = strtok(buffer, "/");

	while(token != NULL){
		if (strcmp(token, ".") == 0){
			// current directory
			token = strtok(NULL, "/");
		}
		else if (strcmp(token, "..") == 0){
			// one up
			it = strlen(absolute) - 2;
			while (absolute[it] != '/' && it >= 0) it--;
			if (it >= 0){
				absolute[it + 1] = '\0';
				token = strtok(NULL, "/");
			}
			else
				token = NULL;
		}
		else {
			strcat(absolute, token);
			strcat(absolute, "/");

			token = strtok(NULL, "/");
		}
	}

	return absolute;
}

BYTE* ReadCluster(int cluster){
	BYTE buffer[clusterSize];
	int it;

	if (cluster < 2 || cluster > clusterCount)
		return NULL;

	cluster -= 2;

	for (it = 0; it < superblock.SectorPerCluster; it++) {
		read_sector(superblock.DataSectorStart + cluster *
					superblock.SectorPerCluster + it,
					(char *)(buffer + it * SECTOR_SIZE));
	}

	return buffer;
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

	char *abs = AbsolutePath(pathname);

	puts(abs);
	return 0;
}

int readdir2(DIR2 handle, DIRENT2 *dentry){
	t2fs_init();

	if (open_files[handle] == NULL ||
			open_files[handle]->TypeVal != TYPEVAL_DIRETORIO)
		return -1;

	//BYTE *buffer = ReadCluster(open_files[handle]->firstCluster);

	return 0;
}

int closedir2(DIR2 handle){
	t2fs_init();
	return 0;
}

int chdir2(char *pathname){
	t2fs_init();

	if (DirExists(pathname) == 1)
		workdir = AbsolutePath(pathname);
	else
		return -1;

	return 0;
}

int getcwd2(char *pathname, int size){
	t2fs_init();
	int length = strlen(workdir);

	if (size >= length)
		memcpy(pathname, workdir, length);
	else
		return -1;

	return 0;
}

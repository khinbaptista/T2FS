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

// Helper functions prototypes
void t2fs_init();
void t2fs_readSuperblock();
int DirExists(char *pathname);
int FileExists(char *pathname);

char* AbsolutePath(char *pathname); // returns the absolute path or NULL on failure

// Functions
int identify2(char* name, int size){
	char *id = "Joao Lauro 195505 | Khin Baptista 217443";	//40

	if (size >= 40)
		memcpy(name, id, 40);
	else
		memcpy(name, id, size);

	t2fs_init();
	return 0;
}

void t2fs_init(){
	if (initialized == 0){
		t2fs_readSuperblock();
	}
}

void t2fs_readSuperblock(){
	BYTE buffer[SECTOR_SIZE];
	read_sector(0, (char *)buffer);

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

	initialized = 1;
}

int DirExists(char *pathname){
	return -1;
}

int FileExists(char *pathname){
	return -1;
}

char* AbsolutePath(char *pathname){
	int wdirlength = strlen(workingdir);
	int it;
	char *token;
	char absolute[wdirlength + strlen(pathname)];

	if (pathname[0] == '/')
		return pathname;

	if (pathname[0] != '.')
		return NULL;

	memcpy(absolute, workingdir, wdirlength);

	token = strtok(pathname, "/");

	while(token != NULL){
		if (strcmp(token, ".") == 0){
			// current directory
			token = strtok(NULL, "/");
		}
		else if (strcmp(token, "..") == 0){
			// one up
			it = strlen(absolute) - 1;
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
		}
	}

	return absolute;
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
	//t2fs_init();

	puts(AbsolutePath(pathname));
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

	if (DirExists(pathname) == 1)
		workingdir = AbsolutePath(pathname);
	else
		return -1;

	return 0;
}

int getcwd2(char *pathname, int size){
	t2fs_init();
	int length = strlen(workingdir);

	if (size >= length)
		memcpy(pathname, workingdir, length);
	else
		return -1;

	return 0;
}

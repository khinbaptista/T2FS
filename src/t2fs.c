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

typedef struct t2fs_record RECORD;

// ########################################
/* Data kept in memory */

int initialized = 0;

struct t2fs_superbloco sb;
BYTE* fat;

int clusterSize;
int clusterCount;
int fatSize;
int fatSectorCount;

// string of the current work directory
char* workdir;

// record of open files
RECORD* open_files[MAX_OPEN_FILES];

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
		strcpy(name, id);
	else
		return 1;
	
	t2fs_init();
	return 0;
}

void t2fs_init(){
	if (initialized == 0){
		workdir = "/";
		t2fs_readSuperblock();
		//t2fs_readFAT();

		initialized = 1;
	}
}

void t2fs_readSuperblock(){
	BYTE buffer[SECTOR_SIZE];
	read_sector(0, (char*)buffer);

	memcpy(&sb.Id,					buffer,			4);
	memcpy(&sb.Version,				buffer + 4,		2);
	memcpy(&sb.SuperBlockSize,		buffer + 6,		2);
	memcpy(&sb.DiskSize,			buffer + 8,		4);
	memcpy(&sb.NofSectors,			buffer + 12,	4);
	memcpy(&sb.SectorPerCluster,	buffer + 16,	4);
	memcpy(&sb.pFATSectorStart,		buffer + 20,	4);
	memcpy(&sb.sFATSectorStart,		buffer + 24,	4);
	memcpy(&sb.RootSectorStart,		buffer + 28,	4);
	memcpy(&sb.DataSectorStart,		buffer + 32,	4);
	memcpy(&sb.NofDirEntries,		buffer + 36,	4);

	clusterSize		= SECTOR_SIZE * sb.SectorPerCluster;

	clusterCount	=
		(int)((sb.DiskSize - sb.DataSectorStart) / clusterSize);

	fatSize 		= clusterCount * 16;
	fatSectorCount	= fatSize / SECTOR_SIZE;

	if (fatSize % SECTOR_SIZE != 0) fatSectorCount++;

	printf("Cluster size: %d\nCluster count: %d\n", clusterSize, clusterCount);
	printf("FAT size: %d, takes %d sectors\n", fatSize, fatSectorCount);
}

void t2fs_readFAT(){
	int it;
	BYTE buffer[SECTOR_SIZE];

	fat = malloc(fatSize);

	for(it = 0; it < fatSectorCount; it++){
		read_sector(sb.pFATSectorStart + it, (char*)buffer);
		memcpy(fat + it * SECTOR_SIZE, buffer, SECTOR_SIZE);
	}
}

WORD FAT(int cluster){
	return fat[(cluster - 2) * 16];
}

int file_exists(char *pathname, BYTE typeVal){
	int it, it2, cluster, found = 0;
	char* path = absolute_path(pathname);
	char* step;

	BYTE *b_sector	= malloc(SECTOR_SIZE);
	BYTE* b_cluster;
	RECORD buffer;

	if (path == NULL || typeVal == TYPEVAL_INVALIDO)
		return 0;

	if  (strlen(path) == 1 && path[0] == '/')
		return 1;

	read_sector(sb.RootSectorStart, (char *)b_sector);
	step = strtok(path, "/");

	for (it = 0; it < sb.NofDirEntries && found == 0; it++){
		memcpy(&buffer, b_sector + it * sizeof(RECORD), sizeof(RECORD));

		if (strcmp(buffer.name, step) == 0 && buffer.TypeVal == typeVal)
			found = 1;
	}

	if (found == 0){
		free(b_sector);
		return 0;
	}

	cluster = buffer.firstCluster;
	step = strtok(NULL, "/");

	while (step != NULL){
		found = 0;

		b_cluster = read_cluster(cluster);

		if (b_cluster == NULL){
			printf("Failure reading cluster %d (looking for %s)\n",
				buffer.firstCluster, path);
			return 0;
		}

		for (it = 0; it < sb.SectorPerCluster && found == 0; it++){
		// for each sector in the cluster
			memcpy(b_sector, b_cluster + it * SECTOR_SIZE, SECTOR_SIZE);

			for (it2 = 0; it2 < SECTOR_SIZE / sizeof(RECORD) && found == 0; it2++){
			// for each entry in the sector
				memcpy(&buffer, b_sector + it2 * sizeof(RECORD), sizeof(RECORD));

				if(strcmp(buffer.name, step) == 0 && buffer.TypeVal == typeVal)
					found = 1;
			}
		}

		if (found == 0){
			free(b_sector);
			free(b_cluster);
			return 0;
		}

		step = strtok(NULL, "/");
		cluster = FAT(cluster);

		if (cluster == 0 || cluster == 1 ||
				((cluster >= clusterCount) && (cluster < 0x0FFFF))){
			free(b_sector);
			free(b_cluster);
			return 0;
		}
	}

	free(b_sector);
	free(b_cluster);

	return found;
}

char* absolute_path(char *pathname){
	int wdirlength = strlen(workdir);
	int pathlength = strlen(pathname);
	int it;
	char* buffer;
	char* token;
	char* absolute;

	if (pathname[0] != '.' && pathname[0] != '/')
		return NULL;
		
	absolute	= malloc(wdirlength + pathlength);
	buffer		= malloc(pathlength);

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

BYTE* read_cluster(int cluster){
	BYTE *buffer = malloc(clusterSize);
	int it;

	if (cluster < 2 || cluster > clusterCount)
		return NULL;

	cluster -= 2;

	for (it = 0; it < sb.SectorPerCluster; it++) {
		read_sector(sb.DataSectorStart + cluster * sb.SectorPerCluster + it,
					(char *)(buffer + it * SECTOR_SIZE));
	}

	return buffer;
}

int generate_handler(){
	int it;
	int index = -1;

	for (it = 0; it < MAX_OPEN_FILES && index == -1; it++){
		if (open_files[it] == NULL)
			index = it;
	}

	return index;
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
	int handler = generate_handler();
	char *path = absolute_path(pathname);

	return 0;
}

int readdir2(DIR2 handle, DIRENT2 *dentry){
	t2fs_init();

	if (open_files[handle] == NULL ||
			open_files[handle]->TypeVal != TYPEVAL_DIRETORIO)
		return -1;

	//BYTE *buffer = read_cluster(open_files[handle]->firstCluster);

	return 0;
}

int closedir2(DIR2 handle){
	t2fs_init();
	return 0;
}

int chdir2(char *pathname){
	t2fs_init();

	/*if (file_exists(pathname, TYPEVAL_DIRETORIO) == 1)
		workdir = absolute_path(pathname);
	else
		return -1;*/

	workdir = absolute_path(pathname);

	return 0;
}

int getcwd2(char *pathname, int size){
	t2fs_init();
	int length = strlen(workdir);

	if (size >= length)
		strcpy(pathname, workdir);
	else
		return -1;
puts(pathname);
	return 0;
}

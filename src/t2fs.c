/* ########################################################################
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
######################################################################## */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "apidisk.h"
#include "t2fs.h"
#include "helper.h"

/* ######################################## */

#define MAX_OPEN_FILES 20

/* ######################################## */
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
RECORD* open_files[MAX_OPEN_FILES] = { NULL };

// one cluster open for each file
BYTE* open_clusters[MAX_OPEN_FILES] = { NULL };

// offset of file inside open clluster
int open_offsetcluster[MAX_OPEN_FILES] = { 0 };

// offset of open file relative to file start
int open_offset[MAX_OPEN_FILES] = { 0 };

/* ######################################## */
/* Functions */

int identify2(char* name, int size){
	char id[] = "Joao Lauro 195505 | Khin Baptista 217443";
	int length = strlen(id);

	if (size >= length)
		strcpy(name, id);
	else
		return -1;

	t2fs_init();
	return 0;
}

void t2fs_init(){
	if (initialized == 0){
		workdir = "/";
		t2fs_readSuperblock();
		t2fs_readFAT();

		initialized = 1;
		puts("T2FS INITIALIZED OK\n");
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
		(int)((sb.DiskSize - sb.DataSectorStart * SECTOR_SIZE) / clusterSize);

	fatSize 		= clusterCount * 16;
	fatSectorCount	= fatSize / SECTOR_SIZE;

	if (fatSize % SECTOR_SIZE != 0) fatSectorCount++;

	printf("\nDisk size: %d\tFirst data sector: %d\n",
		sb.DiskSize, sb.DataSectorStart);

	printf("Sector size: %d\tSectors per cluster: %d\n",
		SECTOR_SIZE, sb.SectorPerCluster);

	printf("Cluster size: %d\tCluster count: %d\n", clusterSize, clusterCount);
	printf("FAT size: %d, takes %d sectors\n", fatSize, fatSectorCount);
}

void t2fs_readFAT(){
	int it;
	BYTE buffer[SECTOR_SIZE];

	fat = calloc(fatSize, sizeof(BYTE));

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
		strcpy(absolute, workdir);

	strcpy(buffer, pathname);
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

int generate_handler(){
	int it;
	int index = -1;

	for (it = 0; it < MAX_OPEN_FILES && index == -1; it++){
		if (open_files[it] == NULL)
			index = it;
	}

	return index;
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

RECORD* find_root_subpath(char* subpath, BYTE typeval){
	RECORD	*result, *buffer;
	BYTE	*sector;

	int i_entry, i_sector	= 0;	// indexes
	int counter 			= 0;	// entries checked
	int root_size			= sb.NofDirEntries * sizeof(RECORD);
	int entries_per_sector	= SECTOR_SIZE / sizeof(RECORD);
	int root_sectors		= (int)(root_size / SECTOR_SIZE);
	if (root_size % SECTOR_SIZE != 0) root_sectors++;

	// alert
	if (SECTOR_SIZE % sizeof(RECORD) != 0) puts("F: find_root_subpath");

	while (result == NULL && counter < sb.NofDirEntries){
		sector = malloc(SECTOR_SIZE);
		read_sector(sb.RootSectorStart + i_sector, (char*)sector);

		for (i_entry = 0; i_entry < entries_per_sector &&
				result == NULL && counter < sb.NofDirEntries; i_entry++){
			buffer = (RECORD*)(sector + i_entry * sizeof(RECORD));

			if (buffer->TypeVal == typeval && strcmp(buffer->name, subpath) == 0){
				result = malloc(sizeof(RECORD));
				memcpy(result, buffer, sizeof(RECORD));
			}

			counter++;
		}

		free(sector);
	}

	return result;
}

int find_record_subpath(RECORD* current, char* subpath, BYTE typeval){
	RECORD	*buffer;
	BYTE	*cluster, *sector;

	int cluster_num = (int)current->firstCluster;
	int i_entry, i_sector, found = 0;
	int entries_per_sector = SECTOR_SIZE / sizeof(RECORD);

	if (current->TypeVal != TYPEVAL_DIRETORIO)
		return 0;

	while (found == 0 && i_sector < sb.SectorPerCluster && cluster_num != 0x0FF){
		cluster = read_cluster(cluster_num);

		for (i_sector = 0; i_sector < entries_per_sector && found == 0; i_sector++){
			sector = malloc(SECTOR_SIZE);
			read_sector(cluster + i_sector, (char*)sector);

			for (i_entry = 0; i_entry < entries_per_sector && found == 0; i_entry++){
				buffer = (RECORD*)(sector + i_entry * sizeof(RECORD));

				if (buffer->TypeVal == typeval && strcmp(buffer->name, subpath) == 0){
					free(current);
					current = malloc(sizeof(RECORD));
					memcpy(current, buffer, sizeof(RECORD))
					found = 1;
				}
			}

			free(sector);
		}

		free(cluster);
		if (found == 0)
			cluster_num = FAT(cluster_num);
	}

	return 0;
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

	int handler = -1;
	char *path;
	char* token;

	RECORD* _record;

	path = absolute_path(pathname);
	if (path == NULL)
		return -1;

	token = strtok(path, "/");
	_record = find_root_subpath(token, TYPEVAL_DIRETORIO);
	if (_record == NULL)	// maybe this is not necessary
		return -1;

	token = strtok(NULL, "/");

	while(token != NULL && _record != NULL){
		find_record_subpath(_record, token, TYPEVAL_DIRETORIO);
		token = strtok(NULL, "/");
	}

	if (token == NULL && _record != NULL){	// found it!
		handler = generate_handler();

		if (handler >= 0){
			open_files[handler] = malloc(sizeof(RECORD));
			memcpy(open_files[handler], _record, sizeof(RECORD));
			open_clusters[handler]		= NULL;
			open_offset[handler]		= 0;
			open_offsetcluster[handler]	= 0;
		}
	}

	free(path);
	if (_record != NULL)	free(_record);
	if (token != NULL)		free(token);

	return handler;
}

int readdir2(DIR2 handle, DIRENT2 *dentry){
	t2fs_init();

	if (handle < 0 || handle >= MAX_OPEN_FILES || open_files[handle] == NULL ||
			open_files[handle]->TypeVal != TYPEVAL_DIRETORIO)
		return -1;

	//...

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

	return 0;
}

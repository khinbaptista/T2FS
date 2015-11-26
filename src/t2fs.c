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
int debug = 1;

struct t2fs_superbloco sb;
BYTE* fat;

int clusterSize = 0;
int clusterCount = 0;
int fatSize = 0;
int fatSectorCount = 0;

// string of the current work directory
char* workdir;

// record of open files
RECORD* open_files[MAX_OPEN_FILES]		= { NULL };

// index of open cluster for each file
int open_cluster_num[MAX_OPEN_FILES]	= { 0 };

// one cluster open for each file
BYTE* open_clusters[MAX_OPEN_FILES]		= { NULL };

RECORD *root;
int root_offset = 0;	// in entries (not bytes)
int root_entries = 0;	// actual entryes (not total space available)

// offset of file inside open cluster
int open_offsetcluster[MAX_OPEN_FILES]	= { 0 };

// offset of open file relative to file start
int open_offset[MAX_OPEN_FILES]			= { 0 };

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
		t2fs_readRoot();

		initialized = 1;
		if (debug == 1) puts("T2FS INITIALIZED OK\n");
	}
}

void t2fs_readSuperblock(){
	BYTE buffer[SECTOR_SIZE] = { 0 };
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

	if (debug == 1) {
		printf("\nDisk size: %d\tFirst data sector: %d\n",
			sb.DiskSize, sb.DataSectorStart);

		printf("Sector size: %d\tSectors per cluster: %d\n",
			SECTOR_SIZE, sb.SectorPerCluster);

		printf("Cluster size: %d\tCluster count: %d\n", clusterSize, clusterCount);
		printf("FAT size: %d, takes %d sectors\n", fatSize, fatSectorCount);
	}
}

void t2fs_readFAT(){
	int it;
	BYTE buffer[SECTOR_SIZE] = { 0 };

	fat = calloc(fatSize, sizeof(BYTE));

	for(it = 0; it < fatSectorCount; it++){
		read_sector(sb.pFATSectorStart + it, (char*)buffer);
		memcpy(fat + it * SECTOR_SIZE, buffer, SECTOR_SIZE);
	}

	if (debug == 1) printf("FAT read.\n");
}

int t2fs_writeFAT(){
	int i, p = 0, s = 0;
	// status of writing Primary and Secondary FATs into disk

	for (i = 0; i < fatSectorCount && p == 0; i++)
		p =
			write_sector(sb.pFATSectorStart + it, (char*)fat + it * SECTOR_SIZE);

	if (p == 0){	// nothing went wrong
		for (i = 0; i < fatSectorCount && s == 0; i++)
			s =
				write_sector(sb.sFATSectorStart + it, (char*)fat + it * SECTOR_SIZE);
	}

	if (s != 0)
		s = -2;

	// -1 means error writing primary FAT
	// -2 means error writing secondary FAT
	return p + s;
}

WORD FAT(int cluster){
	return fat[(cluster - 2) * 16];
}

void t2fs_readRoot(){
	RECORD *record, *next;
	BYTE *buffer;
	int sector_it	= 0;
	int it			= 0;

	root = calloc(sizeof(RECORD), sb.NofDirEntries);
	root_entries = 0;

	buffer = calloc(SECTOR_SIZE, sizeof(char));
	read_sector(sb.RootSectorStart, (char*)buffer);

	for (it = 0; it < sb.NofDirEntries; it++){
		if (sizeof(RECORD) * sector_it >= SECTOR_SIZE){
			read_sector(sb.RootSectorStart + sector_it, (char*)buffer);
			sector_it++;
		}

		record = (RECORD*)(buffer + it * sizeof(RECORD));

		if (record != NULL && record->TypeVal == TYPEVAL_REGULAR ||
				record->TypeVal == TYPEVAL_DIRETORIO){

			next = (RECORD*)(root + root_entries * sizeof(RECORD));

			next->TypeVal = record->TypeVal;
			strcpy(next->name, record->name);
			next->bytesFileSize = record->bytesFileSize;
			next->firstCluster = record->firstCluster;

			root_entries++;

			if (debug == 1)
				printf("\t(read_root) Root dir entry: %s\n", next->name);
		}
	}

	if (debug == 1) printf("Root valid entries: %d\n", root_entries);
}

int write_root(){
	int root_sector_count = 1;
	int status = 0;

	if (root_entries * sizeof(RECORD) > SECTOR_SIZE){
		root_sector_count = (root_entries * sizeof(RECORD)) / SECTOR_SIZE;

		if ((root_entries * sizeof(RECORD)) % SECTOR_SIZE != 0)
			root_sector_count++;
	}

	for (int i = 0; i < root_sector_count && status == 0; i++)
		status = write_sector(sb.RootSectorStart + i,
			(char*)root + i * SECTOR_SIZE);

	return status;
}

int file_exists(char *pathname, BYTE typeVal){
	int handler, exists = 0;

	if (typeVal == TYPEVAL_DIRETORIO){
		handler = opendir2(pathname);

		if (handler >= 0){
			closedir2(handler);
			exists = 1;
		}
	}
	else if (typeVal == TYPEVAL_REGULAR){
		handler = open2(pathname);

		if (handler >= 0){
			close2(handler);
			exists = 1;
		}
	}

	return exists;
}

char* absolute_path(char *pathname){
	int wdirlength = strlen(workdir);
	int pathlength = strlen(pathname);
	int it;
	char buffer[pathlength];
	char* token;
	char* absolute;

	if (pathname[0] != '.' && pathname[0] != '/')
		return NULL;

	absolute	= calloc(wdirlength + pathlength, sizeof(char));

	if (pathname[0] == '/')
		absolute[0] = '\0';
	else
		strcpy(absolute, workdir);

	strcpy(buffer, pathname);
	token = strtok(buffer, "/");

	while(token != NULL){
		strcat(absolute, "/");

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
	BYTE *pointer;
	BYTE *buffer = malloc(clusterSize);
	int it;

	if (cluster < 2 || cluster > clusterCount)
		return NULL;

	cluster -= 2;

	for (it = 0; it < sb.SectorPerCluster; it++) {
		pointer = buffer + it * SECTOR_SIZE;

		read_sector(sb.DataSectorStart + cluster *
					sb.SectorPerCluster + it, (char*)pointer);
	}

	return buffer;
}

int read_next_cluster(int handle){
	int cluster = FAT(open_cluster_num[handle]);

	if (cluster == 0x0FFFF)
		return -1;

	open_cluster_num[handle] = cluster;
	free(open_clusters[handle]);
	open_clusters[handle] = read_cluster(cluster);

	return 0;
}

int write_cluster(int cluster, BYTE* data){
	int status = 0;

	cluster -= 2;

	for (int sector = 0; sector < sb.SectorPerCluster && status == 0; sector++)
		status = write_sector(sb.DataSectorStart + cluster * sb.SectorPerCluster
			+ sector, (char*)data + sector * SECTOR_SIZE);

	return status;
}

int find_free_cluster(){
	int found = 0, i;

	for (i = 2; i < clusterCount && found == 0; i++){
		if (FAT(i) == 0)
			found = 1;
	}


	if (found == 1)
		return i - 1;
	else
		return -1;
}

RECORD* find_root_subpath(char* subpath, BYTE typeval){
	RECORD	*result, *buffer;
	int it, found = 0;

	for (it = 0; it < root_entries && found == 0; it++){
		buffer = (RECORD*)(root + it * sizeof(RECORD));

		if (buffer != NULL && buffer->TypeVal == typeval &&
				strcmp(buffer->name, subpath) == 0){

			found = 1;
			printf("\n\t(find_root_subpath) Found subpath '%s'\n", subpath);

			result = malloc(sizeof(RECORD));
			memcpy(result, buffer, sizeof(RECORD));
		}
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
			read_sector((unsigned int)cluster + i_sector, (char*)sector);

			for (i_entry = 0; i_entry < entries_per_sector && found == 0; i_entry++){
				buffer = (RECORD*)(sector + i_entry * sizeof(RECORD));

				if (debug == 1)
					printf("(find_record_subpath) Entry: %s\n", buffer->name);

				if (buffer->TypeVal == typeval && strcmp(buffer->name, subpath) == 0){
					free(current);
					current = malloc(sizeof(RECORD));
					memcpy(current, buffer, sizeof(RECORD));
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

	if (file_exists(filename, TYPEVAL_REGULAR) == 1)
		return -1; // file already exists

	RECORD file, *record;
	char *path, *name;
	int dir = -1, status = 0, handle;
	DIRENT2 dir_buffer;

	path = absolute_path(filename);
	if (path == NULL) return -1;
	path = absolute_path(strcat(pathm "/../"));
	if (path == NULL) return -1;

	name = absolute_path(filename);
	if (name == NULL) return -1;

	for (int found = 0, i = strlen(name) - 2; i >= 0 && found == 0; i--)
		if (name[i] == '/'){
			found = 1;
			strcpy(name, name[i + 1]);
		}

	file.TypeVal		= TYPEVAL_REGULAR;
	file.firstCluster	= find_free_cluster();
	file.bytesFileSize	= 0;
	strcpy(&file.name, name);

	fat[(file.firstCluster - 2) * 16] = 0x0FF;

	dir = opendir2(path);

	if (dir == MAX_OPEN_FILES){		// root dir is special
		record = root + root_entries * sizeof(RECORD);
		record->TypeVal			= file.TypeVal;
		record->firstCluster	= file.firstCluster;
		record->bytesFileSize	= file.bytesFileSize;
		strcpy(record->name, file.name);

		root_entries++;
		status = write_root();



		if (status != 0)
			return -1;

		handle = generate_handler();
	}

	// point to last entry in dir
	while (readdir2(dir, dir_buffer) == 0);
	open_offsetcluster[dir]	+= sizeof(RECORD);
	open_offset[dir]		+= sizeof(RECORD);

	if (status == 0)
		status = t2fs_writeFAT();

	if (status == 0 && handler >= 0){
		record = open_files[handle];
		record->TypeVal			= file.TypeVal;
		record->firstCluster	= file.firstCluster;
		record->bytesFileSize	= file.bytesFileSize;
		strcpy(record->name, file.name);

		open_cluster_num[handle] = file.firstCluster;
		open_clusters[handle] = read_cluster(file.firstCluster);
		open_offset[handle] = 0;
		open_offsetcluster[handle] = 0;

		return handle;
	}

	return -1;
}

int delete2(char *filename){
	t2fs_init();
	return -1;
}

FILE2 open2(char *filename){
	t2fs_init();
	return -1;
}

int close2(FILE2 handle){
	return -1;
}

int read2(FILE2 handle, char *buffer, int size){
	return -1;
}

int write2(FILE2 handle, char *buffer, int size){
	return -1;
}

int seek2(FILE2 handler, unsigned int offset){
	return -1;
}

int mkdir2(char *pathname){
	t2fs_init();

	RECORD new_dir;
	int free_cluster = find_free_cluster();

	if (free_cluster == -1)
		return -1;

	fat[(free_cluster - 2) * 16] = 0x0FF;

	// Incomplete

	return t2fs_writeFAT();
}

int rmdir2(char *pathname){
	t2fs_init();

	if (file_exists(pathname, TYPEVAL_DIRETORIO) == 0)
		return -1;

	int handle, cluster;

	handle = opendir2(pathname);
	cluster = open_files[handle]->firstCluster;
	closedir2(handle);

	while (cluster != 0x0FF){
		fat[(cluster - 2) * 16] = 0;
		cluster = FAT(cluster);
	}

	return t2fs_writeFAT();
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

	if (debug == 1)
		printf("\t(opendir2) Absolute path is '%s'\n", path);

	token = strtok(path, "/");

	if (token == NULL){
		if (debug == 1)
			printf("\t(opendir2) First token is null, the user is opening root dir\n");

		handler = MAX_OPEN_FILES;
		root_offset = 0;

		return handler;
	}

	_record = find_root_subpath(token, TYPEVAL_DIRETORIO);

	token = strtok(NULL, "/");

	while(token != NULL && _record != NULL){
		find_record_subpath(_record, token, TYPEVAL_DIRETORIO);
		token = strtok(NULL, "/");
	}

	if (token == NULL && _record != NULL){	// found it!
		handler = generate_handler();

		if (handler >= 0){printf("First cluster: %d\n", _record->firstCluster);
			open_files[handler] = malloc(sizeof(RECORD));
			memcpy(open_files[handler], _record, sizeof(RECORD));
			open_cluster_num[handler]	= _record->firstCluster;
			open_clusters[handler]		= read_cluster(_record->firstCluster);
			open_offset[handler]		= 0;
			open_offsetcluster[handler]	= 0;
		}
	}

	free(path);

	if (_record != NULL)	free(_record);
	if (token != NULL)		free(token);

	return handler;
}

int user_read_root(DIRENT2 *dentry){
	RECORD* buffer;

	if (root_offset >= sb.NofDirEntries){
		return -1;
	}

	buffer = (RECORD*)(root + root_offset * sizeof(RECORD));
	strcpy(dentry->name, buffer->name);
	dentry->fileType = buffer->TypeVal - 1;
	dentry->fileSize = (unsigned long)buffer->bytesFileSize;

	return 0;
}

int readdir2(DIR2 handle, DIRENT2 *dentry){
	int eof = 0;

	if (handle == MAX_OPEN_FILES)
		return user_read_root(dentry);

	if (handle < 0 || handle >= MAX_OPEN_FILES || open_files[handle] == NULL ||
			open_files[handle]->TypeVal != TYPEVAL_DIRETORIO)
		return -1;

	if (open_offsetcluster[handle] + sizeof(RECORD) > clusterSize){
		if (open_offset[handle] + sizeof(RECORD) > open_files[handle]->bytesFileSize)
			return -1;

		if (read_next_cluster(handle) == -1) eof = 1;
	}

	if (eof){
		if (debug == 1) printf("(readdir2) EOF!\n");
		return -1;
	}

	// Respect file size
	if (open_offsetcluster[handle] * clusterSize + open_offset[handle] >=
			open_files[handle]->bytesFileSize)
		return -1;

	RECORD* record = (RECORD*)(open_clusters[handle] + open_offsetcluster[handle]);

	if (record == NULL || record->TypeVal == TYPEVAL_INVALIDO){
		puts("F: readdir2 eof");
		return -1;
	}

	open_offsetcluster[handle]	+= sizeof(RECORD);
	open_offset[handle]			+= sizeof(RECORD);

	dentry->fileType = (int)record->TypeVal - 1;
	strcpy(dentry->name, record->name);
	dentry->fileSize = (unsigned long)record->bytesFileSize;

	return 0;
}

int closedir2(DIR2 handle){
	if (handle < 0 || handle >= MAX_OPEN_FILES || open_files[handle] == NULL ||
			open_files[handle]->TypeVal != TYPEVAL_DIRETORIO)
		return -1;

	write_cluster(open_cluster_num[handle], open_clusters[handle]);

	free(open_files[handle]);
	free(open_clusters[handle]);

	return 0;
}

int chdir2(char *pathname){
	t2fs_init();

	if (file_exists(pathname, TYPEVAL_DIRETORIO) == 1)
		workdir = absolute_path(pathname);
	else
		return -1;

	if (debug == 1)
		printf("Working directory changed to '%s'\n", workdir);

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

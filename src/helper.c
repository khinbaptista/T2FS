#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "helper.h"
#include "apidisk.h"
#include "t2fs.h"

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
	int wdirlength = strlen(workingdir);
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
		memcpy(absolute, workingdir, wdirlength);

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

	if (cluster < 0 || cluster > clusterCount)
		return NULL;

	for (it = 0; it < superblock.SectorPerCluster; it++) {
		read_sector(superblock.DataSectorStart + cluster
			* superblock.SectorPerCluster + it, (char *)(buffer + it * SECTOR_SIZE));
	}

	return NULL;
}

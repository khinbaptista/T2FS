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

void t2fs_readSuperBlock(){
	BYTE buffer[SECTOR_SIZE];
	read_sector(0, (char *)buffer);
	
	memcpy(superblock.Id, buffer, 4);
	printf(superblock.Id);
}

void t2fs_init(){
	t2fs_readSuperBlock();
}

int delete2 (char *filename){
	t2fs_init();
	return 0;
}

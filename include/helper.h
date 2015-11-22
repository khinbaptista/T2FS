#pragma once

#include "t2fs.h"

// Initializes the file system
void t2fs_init();

// Read the super block to keep it in memory
void t2fs_readSuperblock();

// Read the primary FAT into memory
void t2fs_readFAT();

// Reads a cluster and returns the data
BYTE* read_cluster(int clusterIndex);

// Checks whether a file exists (0: false; 1: true)
int file_exists(char *pathname, BYTE typeVal);

// Converts a path to absolute path (returns NULL on failure)
char* absolute_path(char *pathname);

// Returns the next available handler or -1 if none is available
int generate_handler();

// Returns the value of a cluster in the FAT
WORD FAT(int cluster);

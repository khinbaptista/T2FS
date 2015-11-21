#pragma once

#include "t2fs.h"

// Initializes the file system
void t2fs_init();

// Read the super block to keep it in memory
void t2fs_readSuperblock();

// Read the primary FAT into memory
void t2fs_readFAT();

// Reads a cluster and returns the data
BYTE* ReadCluster(int clusterIndex);

// Checks whether a directory exists (0: false; 1: true)
int DirExists(char *pathname);

// Checks whether a file exists (0: false; 1: true)
int FileExists(char *pathname);

// Converts a path to absolute path (returns NULL on failure)
char* AbsolutePath(char *pathname);

// Returns the value of a cluster in the FAT
WORD FAT(int cluster);

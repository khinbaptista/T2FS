#pragma once

/* ######################################## */

#include "t2fs.h"

typedef struct t2fs_record RECORD;

/* ######################################## */

// Initializes the file system
void t2fs_init();

// Read the super block to keep it in memory
void t2fs_readSuperblock();

// Read the primary FAT into memory
void t2fs_readFAT();

// Writes FAT into the disk (primary FAT first, then secondary FAT)
int t2fs_writeFAT();

// Read root dir into memory
void t2fs_readRoot();

// use this to read an entry from root (root is special)
int user_read_root(DIRENT2 *dentry);

// Writes root data into disk
int write_root();

// Reads a cluster and returns the data
BYTE* read_cluster(int clusterIndex);

// Reads the next cluster into memory (overwrites previewsly open cluster)
// returns -1 if EOF
int read_next_cluster(int handle);

// Writes an entire cluster
int write_cluster(int cluster, BYTE* data);

// Checks whether a file exists (0: false; 1: true)
int file_exists(char *pathname, BYTE typeVal);

// Searches for a specific entry on root dir (NULL if none matches)
RECORD* find_root_subpath(char* subpath, BYTE typeval);

// Searches for a specific entry in a dir cluster
int find_record_subpath(RECORD* current, char* subpath, BYTE typeval);

// Finds (or not) a subpath in a directory file
// Returns NULL on failure or the required RECORD description
RECORD* find_subpath(char* subpath, int first_cluster);

// Converts a path to absolute path (returns NULL on failure)
char* absolute_path(char *pathname);

char* up_directory(char *pathname);

// Returns the next available handler or -1 if none is available
int generate_handler();

// Returns the value of a cluster in the FAT
WORD FAT(int cluster);

// Returns the index of a free cluster
int find_free_cluster();

#pragma once

#include "apidisk.h"
#include "t2fs.h"

typedef struct t2fs_record Record;

BYTE* ReadCluster(int clusterIndex);
int DirExists(char *pathname);
int FileExists(char *pathname);

char* AbsolutePath(char *pathname); // returns the absolute path or NULL on failure

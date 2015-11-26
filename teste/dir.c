#include <stdlib.h>
#include <stdio.h>

#include "t2fs.h"

int main(int argc, char* argv[]){
	char ids[40];
	DIR2 dir;
	DIRENT2 dentry;

	identify2(ids, 40);
	printf("Authors:\n\t%s\n\n", ids);

	dir = opendir2("/subdir/");
	printf("Directory open: %d\n\n", dir);

	while (readdir2(dir, &dentry) == 0)
		printf("%d: %s\n", dentry.fileType, dentry.name);

	printf("\nClosing dir: %d\n", dir);
	closedir2(dir);
	
	chdir2("/subdir/");
	
	getcwd2(ids, 40);
	printf("Current dir: %s\n", ids);
	
	return 0;
}

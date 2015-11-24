#include <stdlib.h>
#include <stdio.h>

#include "t2fs.h"
#include "helper.h"

int main(int argc, char* argv[]){
	char ids[40];
	DIR2 dir;

	identify2(ids, 40);
	printf("Authors:\n\t%s\n\n", ids);

	dir = opendir2("/subdir/");
	printf("Directory open: %d\n", dir);
	closedir2(dir);
	
	return 0;
}

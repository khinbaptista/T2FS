#include <stdlib.h>
#include <stdio.h>

#include "t2fs.h"

int main(int argc, char* argv[]){
	char ids[40];
	char wd[20];

	identify2(ids, 40);
	printf("\n%s\n", ids);

	chdir2("/dev/nhanham/");
	if (getcwd2(wd, 20) == 0)
		printf("Current working directory is \"%s\"\n\n", wd);

	return 0;
}

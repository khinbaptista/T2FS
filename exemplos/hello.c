#include <stdlib.h>
#include <stdio.h>

#include "t2fs.h"

int main(int argc, char* argv[]){
	char ids[40];
	char wd[20];

	identify2(ids, 40);
	printf("%s\n\n", ids);

	if (getcwd2(wd, 20) == 0)
		printf("Current working directory is \"%s\"", wd);

	return 0;
}

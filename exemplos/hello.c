#include <stdlib.h>
#include <stdio.h>

#include "t2fs.h"

int main(int argc, char* argv[]){
	char ids[40];
	char wd[200];

	identify2(ids, 40);
	printf("\n%s\n", ids);

	chdir2("/dev/nhacasa");
	if (getcwd2(wd, 200) == 0){
		printf("Current working directory is ");
		puts(wd);
	}
	
	chdir2("../raaawr/");
	if (getcwd2(wd, 200) == 0){
		printf("Current working directory is ");
		puts(wd);
	}
	
	return 0;
}

#include <stdlib.h>
#include <stdio.h>

#include "t2fs.h"
#include "helper.h"

int main(int argc, char* argv[]){
	char ids[40];
	char wd[200];

	identify2(ids, 40);
	printf("Authors:\n\t%s\n\n", ids);

	
	
	return 0;
}

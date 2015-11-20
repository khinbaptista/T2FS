#include <stdlib.h>
#include <stdio.h>

#include "t2fs.h"

int main(int argc, char* argv[]){
	char ids[40];

	identify2(ids, 40);
	printf("%s\n\n", ids);
	
	return 0;
}

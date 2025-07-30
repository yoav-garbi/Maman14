#include "prototypes.h"




int main (int argc, char *argv[])
{
	/*int i;*/
	/*FILE *filePointer;*/
	FILE **fileArr;
	
	
	fileArr = getFiles(argc, argv);	/* take in the arguments into the FILEs array */
	if (fileArr == NULL)
		return ERROR;
	
	
	
	closeFiles(argc, fileArr);
	free(fileArr);
	return 0;
}

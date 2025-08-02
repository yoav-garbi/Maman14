#include "prototypes.h"




int main (int argc, char *argv[])
{
	/*int i;*/
	/*FILE *filePointer;*/
	FILE **fileArr;
	lineNode **lineArr = calloc(argc - 1, sizeof(lineNode *)); /* an array of pointers to linked lists. each linked list is like a file because it holds all lines */
	
	
	fileArr = getFiles(argc, argv);	/* take in the arguments into the FILEs array */
	if (fileArr == NULL)
		return ERROR;
	
	
	closeFiles(argc, fileArr);
	free(fileArr);
	return 0;
}

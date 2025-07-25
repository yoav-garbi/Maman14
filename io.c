#include <stdio.h>
#include <stdlib.h>
#include "prototypes.h"


/* take in the arguments into the FILEs array */
FILE **getFiles(int argc, char *argv[])
{
	int i;
	FILE *filePointer;
	FILE **fileArr = malloc(total_num_of_files(argc) * sizeof(FILE));
	
	for (i = 1; i < argc; ++i)
	{
		filePointer = fopen(argv[i], "r");
		
		if (checkFileExistence(filePointer) == ERROR)	/* check if the file has opened succesfuly */
		{
			return NULL;
		}
		
		fileArr[i-1] = filePointer;
	}
	
	return fileArr;
}



/* close all files */
int closeFiles(int argc, FILE **fileArr)
{
	int i;
	
	for (i = 0; i < total_num_of_files(argc)-1; ++i)
	{
		fclose(fileArr[i]);
	}
	
	return 0;
}

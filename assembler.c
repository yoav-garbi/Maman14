#include <stdio.h>
#include <stdlib.h>
#include "prototypes.h"


int main (int argc, char *argv[])
{
	int i;
	FILE *filePointer;
	FILE **fileArr = malloc(4 * sizeof(FILE));
	
	
	/* take in the arguments into the FILEs array */
	for (i = 1; i < argc; ++i)
	{
		filePointer = fopen(argv[i], "r");
		
		if (checkFileExistence(filePointer) == ERROR)
		{
			return ERROR;
		}
		
		fileArr[i-1] = filePointer;
	}
	
	
	
	
	
	
	
	
	
	/* close all files */
	for (i = 0; i < argc-1; ++i)
	{
		fclose(fileArr[i]);
	}
	
	
	
	free(fileArr);
	return 0;
}

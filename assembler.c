#include <stdio.h>
#include <stdlib.h>

#include "errors.h"


int main (int argc, char *argv[])
{
	int i;
	FILE *filePointer;
	FILE *fileArr[argc-1];
	
	
	for (i = 1; i < argc; ++i)
	{
		filePointer = fopen(argv[i], "r");
		
		if (checkFileExistence(filePointer) == ERROR)
		{
			return ERROR;
		}
		
		fileArr[i-1] = filePointer;
	}
	
	return 0;
}

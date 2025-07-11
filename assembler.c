#include <stdio.h>
#include <stdlib.h>

#include "func_prototypes.h"


int main (int argc, char *argv[])
{
	int i;
	FILE *filePointer;
	FILE **fileArr = malloc(4 * sizeof(FILE));
	
	binTree *root;																								/* TEMP */
	
	
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
	
	
	
	
	
	
	root = makeNode("root", 100);																									/* TEMP */
	setL(root, makeNode("node1", 200));
	setR(root, makeNode("node2", 300));
	
	printTree(root);

	
	
	/* close all files */
	for (i = 0; i < argc-1; ++i)
	{
		fclose(fileArr[i]);
	}
	
	
	
	free(fileArr);
	return 0;
}

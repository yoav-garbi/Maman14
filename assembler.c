#include "prototypes.h"

binTree *labelTable;
int lineCounter;



int main (int argc, char *argv[])
{
	/*int i;*/
	/*FILE *filePointer;*/
	FILE **fileArr;
	lineNode **lineArr = malloc((argc-1)*sizeof(lineNode *)); /* an array of pointers to linked lists. each linked list is like a file because it holds all lines */
	labelTable = NULL;
	
	fileArr = getFiles(argc, argv);	/* take in the arguments into the FILEs array */
	if (fileArr == NULL)
		return ERROR;
	
	
	
																																			/* TEMP */
	addNode(&labelTable, "HELLO", 100, CODE, 0, 0);
	addNode(&labelTable, "LABEL", 105, CODE, 0, 0);
	addNode(&labelTable, "STOP", 110, CODE, 0, 0);
	addNode(&labelTable, "ASDFGHJKLASDFGH", 90, EXTERN, 1, 0);
	addNode(&labelTable, "x1", 200, ENTRY, 0, 1);
	addNode(&labelTable, "XYZ3", 50, MAT, 0, 0);
	addNode(&labelTable, "deer", 250, DATE, 0, 0);	
	
																																			/* TEMP */
	addLineNode(&lineArr[0], "0101101101\n", 100);
	addLineNode(&lineArr[0], " STOP 01\n", 101);
	addLineNode(&lineArr[0], "10 HELLO \n", 102);
	addLineNode(&lineArr[0], "1 ASDFGHJKLASDFGH 0\n", 103);
	addLineNode(&lineArr[0], "00 LABEL \n", 104);
	addLineNode(&lineArr[0], "0 x1 1\n", 105);
	addLineNode(&lineArr[0], "1 XYZ3 1\n", 107);
	addLineNode(&lineArr[0], "1 deer 0\n", 108);
	
	
	
	secondPass(argc, argv, fileArr, lineArr);																								/* TEMP */
	
	
	
	
	
	
	
	closeFiles(argc, fileArr); /* close all open files */
	
	/* TODO- free all allocated storage */
	
	return 0;
}

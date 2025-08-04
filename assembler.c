#include "prototypes.h"

binTree *labelTable;
int lineCounter;



int main (int argc, char *argv[])
{
	lineNode **lineArr = NULL;
	char **nameArr =  NULL;
	FILE **fileArr = NULL;
	labelTable = NULL;
	
	
	/* check that source file/s were entered */
	if (check_fileEntered(argc) == ERROR)
		goto cleanUp;
	
	/* initialize lineArr- lineArr is an array of pointers to linked lists. each linked list is like a file because it holds all lines */
	lineArr = calloc(argc-1, sizeof(lineNode *));
	
	/* take in arguments into FILEs array */
	fileArr = getFiles(argc, argv);
	if (fileArr == NULL)
		goto cleanUp;
	
	/* create an array of strings- each one points to the name of a .as file */
	nameArr = make_nameArr(argc, argv);
	if (nameArr == NULL)
		goto cleanUp;
	
																																			/* TEMP */
	/* these act as a temporary "first pass" */
	addNode(&labelTable, "MAIN", 100, CODE, 0, 0);
	addNode(&labelTable, "LOOP", 107, CODE, 1, 0);
	addNode(&labelTable, "END", 121, CODE, 0, 0);
	addNode(&labelTable, "STR", 122, DATA, 0, 0);
	addNode(&labelTable, "LENGTH", 129, DATA, 1, 0);
	addNode(&labelTable, "K", 132, DATA, 0, 0);
	addNode(&labelTable, "M1", 133, DATA, 0, 0);
	addNode(&labelTable, "L3", 0, CODE, 0, 1);
	addNode(&labelTable, "W", 0, CODE, 0, 1);
																																			/* TEMP */
	addLineNode(&lineArr[0], "0000100100\n", 100, 6);
	addLineNode(&lineArr[0], " M1       10\n", 101, 6);
	addLineNode(&lineArr[0], "0010011100\n", 102, 6);
	addLineNode(&lineArr[0], " W        01\n", 103, 6);
	addLineNode(&lineArr[0], "0010110100\n", 104, 7);
	addLineNode(&lineArr[0], "0010000000\n", 105, 7);
	addLineNode(&lineArr[0], " STR      10\n", 106, 7);
	addLineNode(&lineArr[0], "1001000100\n", 107, 8);
	addLineNode(&lineArr[0], " W        01\n", 108, 8);
	addLineNode(&lineArr[0], "1100000000\n", 109, 9);
	addLineNode(&lineArr[0], "1111101100\n", 110, 9);
	addLineNode(&lineArr[0], "0011111100\n", 111, 10);
	addLineNode(&lineArr[0], "0001010000\n", 112, 10);
	addLineNode(&lineArr[0], "0111000100\n", 113, 11);
	addLineNode(&lineArr[0], " K        10\n", 114, 11);
	addLineNode(&lineArr[0], "0000101100\n", 115, 13);
	addLineNode(&lineArr[0], " M1       10\n", 116, 13);
	addLineNode(&lineArr[0], "0011001100\n", 117, 13);
	addLineNode(&lineArr[0], "0000001100\n", 118, 13);
	addLineNode(&lineArr[0], "1010000100\n", 119, 14);
	addLineNode(&lineArr[0], " L3       01\n", 120, 14);
	addLineNode(&lineArr[0], "1111000000\n", 121, 15);
	
	addLineNode(&lineArr[0], "0001100001\n", 122, 16);
	addLineNode(&lineArr[0], "0001100010\n", 123, 16);
	addLineNode(&lineArr[0], "0001100011\n", 124, 16);
	addLineNode(&lineArr[0], "0001100100\n", 125, 16);
	addLineNode(&lineArr[0], "0001100101\n", 126, 16);
	addLineNode(&lineArr[0], "0001100110\n", 127, 16);
	addLineNode(&lineArr[0], "0000000000\n", 128, 16);
	addLineNode(&lineArr[0], "0000000110\n", 129, 17);
	addLineNode(&lineArr[0], "1111110111\n", 130, 17);
	addLineNode(&lineArr[0], "0000001111\n", 131, 17);
	addLineNode(&lineArr[0], "0000010110\n", 132, 18);
	addLineNode(&lineArr[0], "0000000001\n", 133, 19);
	addLineNode(&lineArr[0], "0000000010\n", 134, 19);
	addLineNode(&lineArr[0], "0000000011\n", 135, 19);
	addLineNode(&lineArr[0], "0000000100\n", 136, 19);

	
	if (secondPass(argc, argv, fileArr, lineArr, nameArr) == ERROR)
		goto cleanUp;
	
	
	
	
	cleanUp:
	closeFiles(argc, fileArr); /* close all open files */
	
	/* free all allocated storage */
	freeTree(&labelTable);
	freeListArr(&lineArr, argc-1);
	freeNameArr(&nameArr, argc-1);
	freeFileArr(&fileArr);
	
	return 0;
}

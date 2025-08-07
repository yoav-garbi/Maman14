#include "prototypes.h"

binTree **labelTable;
int *icArr;
int *dcArr;
int lineCounter;
int fileCounter;
lineNode **entryLineArr;




int main (int argc, char *argv[])
{
	int numFiles = argc-1;
	lineNode **lineArr = NULL;
	char **nameArr =  NULL;
	FILE **fileArr = NULL;
	labelTable = NULL;
	icArr = NULL;
	dcArr = NULL;
	entryLineArr = NULL;
	
	
	/* 1) check that source file/s were entered */
	if (check_fileEntered(argc) == ERROR)
		goto cleanUp;
	
	/* 2) initialize relevant structs and arrays */
	if (initializeLabelTables(argc) == ERROR) /* initialize labelTables array */
		goto cleanUp;
	
	lineArr = calloc(numFiles, sizeof(lineNode *)); /* initialize lineArr- lineArr is an array of pointers to linked lists. each linked list is like a file because it holds all lines */
	if (check_allocation(lineArr) == ERROR)
		goto cleanUp;
	
	icArr = calloc(numFiles, sizeof(int)); /* initialize icArr */
	dcArr = calloc(numFiles, sizeof(int)); /* initialize dcArr */
	if (check_allocation(icArr) == ERROR || check_allocation(dcArr) == ERROR)
		goto cleanUp;
	
	entryLineArr = calloc(numFiles, sizeof(lineNode *)); /* initialize lineArr- lineArr is an array of pointers to linked lists. each linked list is like a file because it holds all lines */
	if (check_allocation(entryLineArr) == ERROR)
		goto cleanUp;
	
	
	/* 3) open .as files (store in fileArr) */
	fileArr = getFiles(argc, argv);
	if (fileArr == NULL)
		goto cleanUp;
	
	/* 4) build nameArr- an array of strings, each one is a name of a .as file */
	nameArr = make_nameArr(argc, argv);
	if (nameArr == NULL)
		goto cleanUp;


	/* first pass here */
	int i;
	int firstPassError = 0;
	for (i = 0; i < numFiles; i++) {
    		int res = firstPass(i);
    		if (res != 0) {
        		printf("Error in first pass of file %s (%d errors found)\n", nameArr[i], res);
        		firstPassError = 1;
    		}
	}
	if (firstPassError) {
    		printf("Errors were found in the first pass. Compilation stopped.\n");
    		goto cleanUp;
	}

	

	/* second pass */
	if (secondPass(argc, argv, fileArr, lineArr, nameArr) == ERROR)
		goto cleanUp;
	
	
	
	
	cleanUp:
	closeFiles(argc, fileArr); /* close all open files */
	
	/* free all allocated storage */
	freeLabelTable(&labelTable, argc-1);
	freeListArr(&lineArr, argc-1);
	freeNameArr(&nameArr, argc-1);
	freeFileArr(&fileArr);
	free(icArr);
	free(dcArr);
	freeListArr(&entryLineArr, argc-1);
	
	return 0;
}

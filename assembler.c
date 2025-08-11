#include "prototypes.h"
#include "header.h"

lineNode **lineArr;
char **nameArr;
FILE **fileArr;
binTree **labelTable;
macro **macroArr;
int *icArr;
int *dcArr;
int lineCounter;
int fileCounter;
int macroCounter;
lineNode **entryLineArr;
lineNode **externLineArr;
char ***argvPointer;
int amOffset;


int main (int argc, char *argv[])
{
	int numFiles = argc-1, errorFlag = 0, res;
	labelTable = NULL;
	macroArr = NULL;
	macroCounter = 0;
	icArr = NULL;
	dcArr = NULL;
	entryLineArr = NULL;
	externLineArr = NULL;
	argvPointer = &argv;
	amOffset = numFiles;
	
	
	/* 1) check that source file/s were entered, and are legal */
	if (check_fileEntered(argc) == ERROR || check_fileName(numFiles) == ERROR)
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
	
	entryLineArr = calloc(numFiles, sizeof(lineNode *)); /* initialize entryLineArr- each list holds all entry-d labels from one file */
	if (check_allocation(entryLineArr) == ERROR)
		goto cleanUp;
	
	externLineArr = calloc(numFiles, sizeof(lineNode *)); /* initialize externLineArr- each list holds all raw lines (from one file) that used an external label */
	if (check_allocation(externLineArr) == ERROR)
		goto cleanUp;
	
	if (initializeMacroArr() == ERROR) /* initialize macro array */
		goto cleanUp;
	
	
	
	/* 3) open .as files (store in fileArr) */
	fileArr = getFiles(argc, argv);
	if (fileArr == NULL)
		goto cleanUp;
	
	
	/* 4) build nameArr- an array of strings, each one is a name of a .as file */
	nameArr = make_nameArr(argc, argv);
	if (nameArr == NULL)
		goto cleanUp;
	
	
	/* 5) pre-assembler */
	

	
		
	/* 6) first pass */
	for (fileCounter = 0; fileCounter < numFiles; fileCounter++)
	{
    		lineCounter = 0;
			res = firstPass(fileCounter);
    		if (res != 0) {
        		printf("Error in first pass of file %s (%d errors found)\n", nameArr[fileCounter], res);
        		errorFlag = 1;
    		}
	}
	
	if (errorFlag)
	{
    		printf("\nErrors were found in the first pass. Compilation terminated\n");
    		goto cleanUp;
	}
	
	
	
	/* 7) second pass */
	if (secondPass(argc, argv, fileArr, lineArr, nameArr) == ERROR)
		goto cleanUp;
	
	
	
	/* 8) cleanup everything- close all files and free all memory */
	cleanUp:
	closeFiles(argc, fileArr); /* close all open files */
	
	/* free all allocated storage */
	freeLabelTable(&labelTable, numFiles);
	freeListArr(&lineArr, numFiles);
	freeNameArr(&nameArr, numFiles);
	freeMacroArr();
	freeFileArr(&fileArr);
	free(icArr);
	free(dcArr);
	freeListArr(&entryLineArr, numFiles);
	freeListArr(&externLineArr, numFiles);
	
	return 0;
}

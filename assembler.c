#include "prototypes.h"

binTree **labelTable;
int lineCounter;
int fileCounter;



int main (int argc, char *argv[])
{
	int numFiles = argc-1;
	lineNode **lineArr = NULL;
	char **nameArr =  NULL;
	FILE **fileArr = NULL;
	labelTable = NULL;
	
	
	/* 1) check that source file/s were entered */
	if (check_fileEntered(argc) == ERROR)
		goto cleanUp;
	
	/* 2) initialize labelTables array */
	if (initializeLabelTables(argc) == ERROR)
		goto cleanUp;
	
	/* 3) initialize lineArr- lineArr is an array of pointers to linked lists. each linked list is like a file because it holds all lines */
	lineArr = calloc(numFiles, sizeof(lineNode *));
	
	/* 4) open .as files (store in fileArr) */
	fileArr = getFiles(argc, argv);
	if (fileArr == NULL)
		goto cleanUp;
	
	/* 5) build nameArr- an array of strings, each one is a name of a .as file */
	nameArr = make_nameArr(argc, argv);
	if (nameArr == NULL)
		goto cleanUp;
	
/* TEMP */
	/* ─── Revised TEMP first-pass stub ───────────────────────────────── */
for (fileCounter = 0; fileCounter < numFiles; ++fileCounter) {
    if (fileCounter == 0) {
        /* PROG1.AS directives */
        /* Definitions (must match your addLineNode addresses) */
        addNode(&labelTable[0], "MAIN",   100, CODE, 0, 0);
        addNode(&labelTable[0], "END",    121, CODE, 0, 0);
        addNode(&labelTable[0], "STR",    122, DATA, 0, 0);
        addNode(&labelTable[0], "K",      132, DATA, 0, 0);
        addNode(&labelTable[0], "M1",     133, DATA, 0, 0);

        /* Stub labels defined in this file */
        addNode(&labelTable[0], "LOOP",   102, CODE, 0, 0);
        addNode(&labelTable[0], "LENGTH", 131, DATA, 0, 0);

        /* .entry for LOOP & LENGTH */
        fileCounter = 0;
        addEntryLocal("LOOP");
        addEntryLocal("LENGTH");

        /* .extern for L3 & W */
        addExternAcross("L3",   CODE, numFiles, 0);
        addExternAcross("W",    DATA, numFiles, 0);
    }
    else if (fileCounter == 1) {
        /* PROG2.AS directives */
        /* Definitions */
        addNode(&labelTable[1], "L3",  100, CODE, 0, 0);
        addNode(&labelTable[1], "W",   102, CODE, 0, 0);

        /* .entry for L3 & W */
        fileCounter = 1;
        addEntryLocal("L3");
        addEntryLocal("W");

        /* .extern for LOOP & LENGTH */
        addExternAcross("LOOP",   CODE, numFiles, 1);
        addExternAcross("LENGTH", DATA, numFiles, 1);
    }
}
/* ──────────────────────────────────────────────────────────── */


	/* ─── NEW: wire up lineArr[0] with addLineNode calls ───────── */
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
    
    addLineNode(&lineArr[1], "0011111100\n", 100, 6);
    addLineNode(&lineArr[1], "0010010100\n", 101, 6);
    addLineNode(&lineArr[1], "1111000000\n", 102, 7);
    
    /* ──────────────────────────────────────────────────────────── */
	
	
	
	
	if (secondPass(argc, argv, fileArr, lineArr, nameArr) == ERROR)
		goto cleanUp;
	
	
	
	
	cleanUp:
	closeFiles(argc, fileArr); /* close all open files */
	
	/* free all allocated storage */
	freeLabelTable(&labelTable, argc-1);
	freeListArr(&lineArr, argc-1);
	freeNameArr(&nameArr, argc-1);
	freeFileArr(&fileArr);
	
	return 0;
}

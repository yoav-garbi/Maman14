#include "prototypes.h"

binTree *labelTable;
int lineCounter;



int main (int argc, char *argv[])
{
	FILE **fileArr;
	lineNode **lineArr;
	char **nameArr;
	
	labelTable = NULL;
	
	if (check_fileEntered(argc) == ERROR)
		return ERROR;
		
	lineArr = calloc(argc-1, sizeof(lineNode *)); /* an array of pointers to linked lists. each linked list is like a file because it holds all lines */
	
	fileArr = getFiles(argc, argv);	/* take in the arguments into the FILEs array */
	if (fileArr == NULL)
		return ERROR;
	
	/* create an array of strings- each one points to the name of a .as file */
	nameArr = make_nameArr(argc, argv);
	if (nameArr == NULL)
		return ERROR;
	
																																			/* TEMP */
	addNode(&labelTable, "MAIN", 100, CODE, 0, 0);
	addNode(&labelTable, "LOOP", 107, CODE, 1, 0);
	addNode(&labelTable, "END", 121, CODE, 0, 0);
	addNode(&labelTable, "STR", 122, DATE, 0, 0);
	addNode(&labelTable, "LENGTH", 129, DATE, 1, 0);
	addNode(&labelTable, "K", 132, DATE, 0, 0);
	addNode(&labelTable, "M1", 133, DATE, 0, 0);
	addNode(&labelTable, "L3", 0, CODE, 0, 1);
	addNode(&labelTable, "W", 0, CODE, 0, 1);
																																			/* TEMP */
	addLineNode(&lineArr[0], "0000100100\n", 100);
	addLineNode(&lineArr[0], " M1       10\n", 101);
	addLineNode(&lineArr[0], "0010011100\n", 102);
	addLineNode(&lineArr[0], " W        01\n", 103);
	addLineNode(&lineArr[0], "0010110100\n", 104);
	addLineNode(&lineArr[0], "0010000000\n", 105);
	addLineNode(&lineArr[0], " STR      10\n", 106);
	addLineNode(&lineArr[0], "1001000100\n", 107);
	addLineNode(&lineArr[0], " W        01\n", 108);
	addLineNode(&lineArr[0], "1100000000\n", 109);
	addLineNode(&lineArr[0], "1111101100\n", 110);
	addLineNode(&lineArr[0], "0011111100\n", 111);
	addLineNode(&lineArr[0], "0001010000\n", 112);
	addLineNode(&lineArr[0], "0111000100\n", 113);
	addLineNode(&lineArr[0], " K        10\n", 114);
	addLineNode(&lineArr[0], "0000101100\n", 115);
	addLineNode(&lineArr[0], " M1       10\n", 116);
	addLineNode(&lineArr[0], "0011001100\n", 117);
	addLineNode(&lineArr[0], "0000001100\n", 118);
	addLineNode(&lineArr[0], "1010000100\n", 119);
	addLineNode(&lineArr[0], " L3       01\n", 120);
	addLineNode(&lineArr[0], "1111000000\n", 121);
	
	addLineNode(&lineArr[0], "0001100001\n", 122);
	addLineNode(&lineArr[0], "0001100010\n", 123);
	addLineNode(&lineArr[0], "0001100011\n", 124);
	addLineNode(&lineArr[0], "0001100100\n", 125);
	addLineNode(&lineArr[0], "0001100101\n", 126);
	addLineNode(&lineArr[0], "0001100110\n", 127);
	addLineNode(&lineArr[0], "0000000000\n", 128);
	addLineNode(&lineArr[0], "0000000110\n", 129);
	addLineNode(&lineArr[0], "1111110111\n", 130);
	addLineNode(&lineArr[0], "0000001111\n", 131);
	addLineNode(&lineArr[0], "0000010110\n", 132);
	addLineNode(&lineArr[0], "0000000001\n", 133);
	addLineNode(&lineArr[0], "0000000010\n", 134);
	addLineNode(&lineArr[0], "0000000011\n", 135);
	addLineNode(&lineArr[0], "0000000100\n", 136);

	
	secondPass(argc, argv, fileArr, lineArr, nameArr);																						/* TEMP */
	
	
	
	
	
	
	closeFiles(argc, fileArr); /* close all open files */
	
	/* TODO- free all allocated storage */
	
	return 0;
}

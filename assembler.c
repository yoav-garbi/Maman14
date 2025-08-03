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
	
	
	
	secondPass(argc, argv, fileArr, lineArr, nameArr);																						/* TEMP */
	
	
	
	
	
	
	closeFiles(argc, fileArr); /* close all open files */
	
	/* TODO- free all allocated storage */
	
	return 0;
}

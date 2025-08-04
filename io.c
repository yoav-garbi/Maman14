#include "prototypes.h"



/* take in the arguments into the FILEs array */
FILE **getFiles(int argc, char *argv[])
{
	int i;
	FILE *filePointer;
	FILE **fileArr = calloc(total_num_of_files(argc), sizeof(FILE *));
	
	if (check_allocation(fileArr) == ERROR)
		return NULL;
	
	/* open .as files */
	for (i = 0; i < argc - 1; ++i)
	{
		filePointer = fopen(argv[i+1], "r");
		
		if (check_fileExistence(filePointer) == ERROR)	/* check if the file has opened succesfuly */
			return NULL;
		
		fileArr[i] = filePointer;
	}
	
	return fileArr;
}

char **make_nameArr(int argc, char *argv[])
{
	int i;
	char **nameArr;
	
	/* create an array of strings- each one points to the name of a .as file */
	nameArr = calloc(argc - 1, sizeof(char *));
	if (check_allocation(nameArr) == ERROR)
		return NULL;
	
	for (i = 0; i < argc - 1; ++i) /* argc-1 because the 0th index refers to "./assembler" so we need to skip it without going out of bounds */
	{
		nameArr[i] = malloc(buffer_size * sizeof(char));
		if (check_allocation(nameArr[i]) == ERROR)
			return NULL;
		
		strcpy(nameArr[i], argv[i+1]);	/* argv+1 because the 0th index refers to "./assembler" which is irelevent here, so we need to skip it */
	}
	
	return nameArr;
}

/* create the output files and store in the FILEs array */
int create_amFile(int argc, FILE **fileArr, char **nameArr, int i)
{
	int len;
	FILE *filePointer;
	
	len = strlen(nameArr[i]);
	nameArr[i][len-2] = 'a';
	nameArr[i][len-1] = 'm';

	filePointer = fopen(nameArr[i], "w+");

	if (check_newFileExistence(filePointer) == ERROR)
		return ERROR;

	fileArr[(argc-1) + i] = filePointer;
	printf("\t%s\n", nameArr[i]);																						/*TEMP*/
	
	return 0;
}

/* create ob file and store in the FILEs array */
int create_obFile(int argc, FILE **fileArr, char **nameArr, int i)
{
	int len;
	FILE *filePointer;
	
	len = strlen(nameArr[i]);
	nameArr[i][len-2] = 'o';
	nameArr[i][len-1] = 'b';

	filePointer = fopen(nameArr[i], "w+");

	if (check_newFileExistence(filePointer) == ERROR)
		return ERROR;

	fileArr[2*(argc-1) + i] = filePointer;
	printf("\t%s\n", nameArr[i]);																						/*TEMP*/
	
	return 0;
}

/* create ent file and store in the FILEs array */
int create_entFile(int argc, FILE **fileArr, char **nameArr, int i)
{
	int len;
	FILE *filePointer;
	
	len = strlen(nameArr[i]);
	nameArr[i][len-2] = 'e';
	nameArr[i][len-1] = 'n';
	nameArr[i][len] = 't';
	nameArr[i][len+1] = '\0';

	filePointer = fopen(nameArr[i], "w+");

	if (check_newFileExistence(filePointer) == ERROR)
		return ERROR;

	fileArr[3*(argc-1) + i] = filePointer;
	printf("\t%s\n", nameArr[i]);																						/*TEMP*/
	
	return 0;
}

/* create ext file and store in the FILEs array */
int create_extFile(int argc, FILE **fileArr, char **nameArr, int i)
{
	int len;
	FILE *filePointer;
	
	len = strlen(nameArr[i]);
	nameArr[i][len-3] = 'e';
	nameArr[i][len-2] = 'x';
	nameArr[i][len-1] = 't';
	nameArr[i][len] = '\0';

	filePointer = fopen(nameArr[i], "w+");

	if (check_newFileExistence(filePointer) == ERROR)
		return ERROR;

	fileArr[4*(argc-1) + i] = filePointer;
	printf("\t%s\n", nameArr[i]);																						/*TEMP*/
	
	return 0;
}



/* close all files */
int closeFiles(int argc, FILE **fileArr)
{
	int i;
	
	for (i = 0; i < total_num_of_files(argc); ++i)
		if (fileArr[i] != NULL)
			fclose(fileArr[i]);
	
	return 0;
}



/* take in the next line of text to the buffer */
int takeInLine(char buffer[], FILE *source)
{
	void *status;
	
	status = fgets(buffer, buffer_size, source);
	lineCounter++;
	
	if (status == NULL)	/* fgets returns NULL if the entire input is EOF (an empty line) */
		return EOF_only_line;
	
	if (check_lineLength(buffer) == ERROR)
		return ERROR;
	
	return 0;
}



/* read lines until you reach a non-note and non-white-space-only line */
int skipNotesAndWhiteLines(char buffer[], FILE *source)
{
	int i, count, status;
	char c;
	
	do
	{
		status = takeInLine(buffer, source);	/* take line into buffer */
		if (status == EOF_only_line)	/* if encountered a line that only has EOF then end the task */
			break;
		
		if (status == ERROR)	/* if takeInLine returned error this means that the line is too long, so we can skip it */
			continue;
		
		c = buffer[0];
		if (c == ';')
			continue;
		
		for (i = 0, count = 0; c != EOF && c != '\0'; ++i)	/* read the line with c and mark count with 1 if encountering a non-white-space (this means a real line) */
		{
			c = buffer[i];
			
			if (c != ' ' || c != '\t' || c != '\n')
				count = 1;
		}
		
	} while (count == 0);	/* perform this action as long as count is 0 (as long as the lines are blank) */
	
	return 0;
}



int recognize_opcode(char *code)
{
	int i, status;
	
	for (i = 0; strcmp(code, opcodeTable[i].name); ++i);

	status = check_opcodeName(i);
	if(status == 0)	/* command found- return index */
		return i;
	
	return ERROR;
}



int writeEnt(FILE *file, binTree *root)
{
	char address[address_binary_representation_size+1];	/* +1 is for '\0' */
	
	if (root == NULL)
		return 0;
	
	writeEnt(file, root->left);
	
	if (root->isEntry)
	{
		base10_to_base2_forAddress(root->address, address);
	
		fprintf(file, "%s\t\t", root->str);
		base2_to_base4_strToFile(address, file);
		fprintf(file, "\n");
	}
	
	writeEnt(file, root->right);
	
	return 0;
}


int writeExt(FILE *file, binTree *root)
{
	char address[address_binary_representation_size+1];	/* +1 is for '\0' */
	
	if (root == NULL)
		return 0;
	
	writeExt(file, root->left);
	
	if (root->isExternal)
	{
		base10_to_base2_forAddress(root->address, address);
			
		fprintf(file, "%s\t\t", root->str);
		base2_to_base4_strToFile(address, file);
		fprintf(file, "\n");
	}
	
	writeExt(file, root->right);
	
	return 0;
}

#include "prototypes.h"

extern opcd opcodeTable[16];



/* take in the arguments into the FILEs array */
FILE **getFiles(int argc, char *argv[])
{
	int i;
	FILE *filePointer;
	FILE **fileArr = malloc(total_num_of_files(argc) * sizeof(FILE));
	
	for (i = 1; i < argc; ++i)
	{
		filePointer = fopen(argv[i], "r");
		
		if (check_fileExistence(filePointer) == ERROR)	/* check if the file has opened succesfuly */
		{
			return NULL;
		}
		
		fileArr[i-1] = filePointer;
	}
	
	return fileArr;
}



/* close all files */
int closeFiles(int argc, FILE **fileArr)
{
	int i;
	
	for (i = 0; i < total_num_of_files(argc)-1; ++i)
	{
		fclose(fileArr[i]);
	}
	
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

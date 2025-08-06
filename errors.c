#include "prototypes.h"
#include "header.h"



/* go through a command line and search for all errors that may lie in it */
int check_lineGeneral(char *line)
{
	int status, charsRead;
	char word[MAX_LINE_LENGTH];
	
	/* check if the label is legal */
	sscanf(line, "%s%n", word, &charsRead);
	line += charsRead;
	status = isLabel(word);
	
	if (status == 1)																						/* switch 1 to ERROR in isLabel */
	{
		lineCounter++;
		return ERROR;
	}
	
	/* check if command is legal */
	sscanf(line, "%s%n", word, &charsRead);
	line += charsRead;
	
	status = recognize_opcode(word);
	if (status == ERROR)
		return ERROR;
		
	/*TODO- add handeling for ilegal addressing and excessive text before/in/after line*/
	
	return 0;
}



/* check if a file with a given pointer exists (opened succesfuly) */
int check_fileExistence(void *pointer)
{
	if (pointer == NULL)
	{
		printf("\nSource file does not exist.\n\n");
		return ERROR;
	}
	
	return 0;
}

/* check if a file with a given pointer exists (created succesfuly) */
int check_newFileExistence(void *pointer)
{
	if (pointer == NULL)
	{
		printf("\nFile failed to be created.\n\n");
		return ERROR;
	}
	
	return 0;
}


int check_fileEntered(int argc)
{
	if (argc > 1)
		return 0;
	
	printf("\nNo files entered.\n\n");
	return ERROR;
}


/* check if a command in the source code was mis-written */
int check_opcodeName(int index)
{
	if (index < num_of_opcodes) /* a command was found */
		return 0;
	
	printf("\nUnknown command name. (Line %d)\n\n", lineCounter);
	return ERROR;
}




/* check if a given opcode was given an illegal addressing method in the source code */
int check_legalAddressing(int opcodeIndex, int sourceMethod, int destMethod)
{
	int *permissions1, *permissions2, status1, status2;
	opcd opcodeTable[num_of_opcodes];
	
	permissions1 = opcodeTable[opcodeIndex].source;
	permissions2 = opcodeTable[opcodeIndex].dest;
	
	status1 = permissions1[sourceMethod] == 1;
	status2 = permissions2[destMethod] == 1;
	
	if (status1 && status2)	/* both methods are legal for this opcode */
		return 0;
	
	printf("\nIlegal addressing method for this command. (Line %d)\n\n", lineCounter);
	return ERROR;
}



int check_lineLength(char buffer[])
{
	int len = strlen(buffer);
	int status = len > 0 && len <= MAX_LINE_LENGTH;
	
	if (len == MAX_LINE_LENGTH && buffer[80] != '\n');	/* this check makes sure that a line that has 81 chars and EOF doesn't go unnoticed */
	
	else if (status)
		return 0; /* this means that the line has legal length */
	
	printf("\nLine exceeding the allowed length of 80 chars. (Line %d)\n\n", lineCounter);
	return ERROR;
}


int check_registerNumber(char name[])
{
	if (name[0] == 'r' && (name[1] >= '0' && name[1] <= '7'))
		return 0; /* register name is correct */
	
	printf("\nRegister with this name doesn't exist. (Line %d)\n\n", lineCounter);
	return ERROR;
}



int check_labelLength(char buffer[])
{
	int len = strlen(buffer);
	int status = len > 0 && len <= MAX_LABEL_LENGTH;
	
	if (status)
		return 0; /* this means that the label has legal length */
	
	printf("\nLabel exceeding the allowed length of 30 chars. (Line %d)\n\n", lineCounter);
	return ERROR;	
}



int check_labelName(char *ptr, int len)
{
	if (!isalpha(ptr[0]))
	{
		printf("\nLabel name starts with non-letter. (Line %d)\n\n", lineCounter);
		return ERROR;
	}
	
	if (ptr[len] != ':')
	{
		printf("\nMissing ':' at the end of label. (Line %d)\n\n", lineCounter);
		return ERROR;
	}
	
	return 0;
}



int check_allocation(void *pointer)
{
	if (pointer != NULL)	/* check if memory was allocated succesfuly */
		return 0;	
	
	printf("\nMemory allocation failed.\n\n");
	return ERROR;
}


int check_existsInOtherFileAsEntry(char *str, int fileNum)
{
	int i;
	binTree *node;
	
	for (i = 0; i < fileNum; ++i)
	{
		if (i == fileCounter) /* skip the file where the entry comes from, because it always exists there */
			continue;
		
		node = search(labelTable[i], str);
		if (node != NULL && node->isEntry)
			return 1;
	}
	
	return 0;
}


int check_labelExist_or_legalExternalUse(binTree *node, char *label, lineNode *line, int fileNum)
{
	if (node != NULL)	/* label found locally or as extern placeholder */
		return 0;
	
	
	if (check_existsInOtherFileAsEntry(label, fileNum))
	{
		printf("\nLabel \"%s\" is illegally external for this file. (Line %d)\n\n", label, line->lineNum);
		return ERROR;
	}
	
	/* else- label wasn't found in the label table */
	printf("\nLabel \"%s\" not defined in the assembly file. (Line %d)\n\n", label, line->lineNum);
	return ERROR;
}


int check_entryWithLocalDefinition(binTree *node, char *str)
{
	if (node == NULL)
	{
		printf("\nEntry \"%s\" doesn't have a local definition in this file. (Line %d, File %d)\n\n", str, lineCounter, fileCounter);
		return ERROR;
	}
	
	return 0;
}


int check_labelDuplicate(char *str)
{
	printf("\nLabel \"%s\" was declared multiple times. (Line %d)\n\n", str, lineCounter);
	return ERROR;
}






















																																		/* TEMP- this is Tomer's */
int isLabel(const char *ptr) {
    int len = 0;
    while (ptr[len] && !isspace((unsigned char)ptr[len]) && ptr[len] != ':') {
        len++;
    }
    if (check_labelName((char *)ptr, len) == 0) {
        return 1;
    }
    return 0;
}

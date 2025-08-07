#include "prototypes.h"
#include "header.h"



/* go through a command line and search for all errors that may lie in it */
int check_lineGeneral(char *line, int addressingMethod_opernad1, int addressingMethod_opernad2)
{
	int status, charsRead, command_num, offsetCount = 0;
	char word[MAX_LINE_LENGTH], *legalStart1, *legalStart2, *legalStart3;
	
	
	/* check for ilegal line length */
	if (check_lineLength(line) == ERROR)
		return ERROR;
	
	/* check for extrenous text (chars that are not an .entry/.extern or label or command) */
	sscanf(line, "%s%n", word, &charsRead);
	offsetCount += charsRead;
	
	legalStart1 = strstr(word, ":");
	legalStart2 = strstr(word, ".entry");
	legalStart3 = strstr(word, ".extern");
	/* TODO- add checks for mat and data */
	
	if ((legalStart1 == NULL && legalStart2 == NULL && legalStart3 == NULL) || (legalStart1 == word || legalStart2 == word || legalStart3 == word)) /* start of the line isn't a label, .entry or .extern, or it is one of these, but with garbage text stuck to it (legalStart doesn't point to the same place as word) */
	{
		/* check if it's a command */
		for (command_num = 0; command_num < num_of_opcodes; ++command_num)
			if (strcmp(word, opcodeTable[command_num].name) == 0)
				break;
		
		if (command_num == num_of_opcodes) /* no label, no entry, no extern, and now also no command were found- this is garbage text */
		{
			printf("\nExtrenous text before line. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
	}
	
	
	
	/* check for missing white space before any other text */
	if (legalStart1 != NULL) /* in case- label */
	{
		if (strncmp(legalStart1, ": ", 2) != 0 && strncmp(legalStart1, ":\t", 2) != 0) /* non-white space imidietly after the lable */
		{
			printf("\nMissing white space after label. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
	}
	
	if (legalStart2 != NULL) /* in case- entry */
	{
		if (strncmp(word, ".entry ", 7) != 0 && strncmp(word, ".entry\t", 7) != 0) /* non-white space imidietly after the entry */
		{
			printf("\nMissing white space after entry. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
	}
	
	if (legalStart3 != NULL) /* in case- extern */
	{
		if (strncmp(word, ".extern ", 7) != 0 && strncmp(word, ".extern\t", 7) != 0) /* non-white space imidietly after the extern */
		{
			printf("\nMissing white space after external decleration. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
	}
	
	
	
	/* if label- check if the label is legal (all checks already called by isLabel) */
	if (legalStart1 != NULL)
	{
		status = isLabel(word);
	
		if (status == ERROR)
			return ERROR;
	}
	
	
	/* TODO- check for extrenous text */
	sscanf(line, "%s%n", word, &charsRead);
	offsetCount += charsRead;
	
	
	
	
	/* check if command is legal */
	sscanf(line, "%s%n", word, &charsRead);
	offsetCount += charsRead;
	
	status = recognize_opcode(word);
	if (status == ERROR)
		return ERROR;
	
	/* TODO- check for ilegal ',' */
	
	
	/* TODO- check for ilegal addressing for operand1 */
	
	/* TODO- check for ',' */
	
	/* TODO- check for ilegal addressing for operand2 */
	
	/* TODO- check for extrenous text */
	
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


int check_registerNumber(char name[3])
{
	if (name[0] == 'r' && (name[1] >= '0' && name[1] <= '7') && name[2] == '\0')
		return 0; /* register name is correct */
	
	printf("\nRegister with this name doesn't exist. (Line %d)\n\n", lineCounter);
	return ERROR;
}




int check_labelName(char *ptr)	/* ptr entered should be "(labelStr):\0" */
{
	int len = strlen(ptr), i;
	
	if (len == 1)
	{
		printf("\nLabel is blank. (Line %d)\n\n", lineCounter);
		return ERROR;	
	}
	
	if (len > MAX_LABEL_LENGTH)
	{
		printf("\nLabel exceeding the allowed length of 30 chars. (Line %d)\n\n", lineCounter);
		return ERROR;	
	}
	
	if (!isalpha(ptr[0]))
	{
		printf("\nLabel name starts with non-letter. (Line %d)\n\n", lineCounter);
		return ERROR;
	}
	
	if (ptr[0] == 'r' && ptr[1] >= '0' && ptr[1] <= '7' && ptr[2] == ':')
	{
		printf("\nLabel name is the name of a register. (Line %d)\n\n", lineCounter);
		return ERROR;
	}
	
	for (i = 0; i < num_of_opcodes; ++i)
		if (strcmp(ptr, opcodeTable[i].name) == 0)
		{
			printf("\nLabel name is the name of a command. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
	
	if (strcmp(ptr, ".entry") == 0)
	{
		printf("\nLabel name cannot be \".entry\". (Line %d)\n\n", lineCounter);
		return ERROR;
	}
	
	if (strcmp(ptr, ".extern") == 0)
	{
		printf("\nLabel name cannot be \".extern\". (Line %d)\n\n", lineCounter);
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
		printf("\nLabel \"%s\" is an entry form another file but not imported with .extern in this file- unuseable here.  (Line %d)\n\n", label, line->lineNum);
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


int check_isExternalLabelDefinedInOtherFile(char *label, int numFiles)
{
	int i, type, found = 0;
	binTree *def;
	
	for (i = 0; i < numFiles; ++i)
	{
		if (i == fileCounter)
			continue;
		
		def = search(labelTable[i], label);
		
		if (def != NULL) /* there is an actual decleration of the label in another file */
		{
			type = def->symbolType;
			found = 1;
			break;
		}
	}
	
	if (found != 1)
	{
		printf("\nEntry \"%s\" doesn't have a definition in any file. (Line %d)\n\n", label, lineCounter);
		return ERROR;
	}	
	
	return type;
}

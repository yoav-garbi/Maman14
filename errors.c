#include "prototypes.h"



/* go through a command line and search for all surface level errors that may lie in it */
int check_lineGeneral(char *line)
{
	int status, charsRead, firstWord, secondWord, matHeight = 0, matLength = 0, capacity, wordType;
	char word[MAX_LINE_LENGTH], *c;
	
	
	/*  check Line Length */
	if (check_lineLength(line) == ERROR)
		return ERROR;
	
	/* check for garbage text and clasify the first word */
	firstWord = check_garbageTextBeforeLine(line, &matHeight, &matLength);
	if (firstWord == ERROR)
		return ERROR;
	else if (firstWord == EMPTY_LINE)
		return 0;
	sscanf(line, "%s%n", word, &charsRead);
	line += charsRead; /* advance through the line (skip first word now that it's checked) */
	
	
	/* take in next word in the case that the first word was a label, entry or extern */
	if (firstWord == wordIsLABEL || firstWord == wordIsENTRY || firstWord == wordIsEXTERN)
	{
		secondWord = check_garbageTextAndClassifyWord(line, firstWord, &matHeight, &matLength);
		if (secondWord == ERROR)
			return ERROR;
		
		if ((firstWord == wordIsENTRY || firstWord == wordIsEXTERN) && secondWord == EMPTY_LINE)
		{
			printf("\nMissing label after directive. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		if (firstWord == wordIsLABEL && secondWord == EMPTY_LINE)
		{
			printf("\nMissing code/data after label declaration. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		if (secondWord == EMPTY_LINE)
			return 0;
		
		sscanf(line, "%s%n", word, &charsRead);
		line += charsRead; /* advance through the line (skip second word now that it's checked) */
		
		wordType = secondWord;
		
		/* .entry/.extern must end after the label */
		if (firstWord == wordIsENTRY || firstWord == wordIsEXTERN)
		{
			c = skipWhiteSpace(line);
			
			if (!isEndOfLine_or_whiteSpaceOnly(c))
			{
				printf("\nExtraneous text after line. (Line %d)\n\n", lineCounter);
				return ERROR;
			}
			return 0; /* nothing more to check in this line */
		}
	}
	
	else /* set wordType in case of non: label, entry or extern */
		wordType = firstWord;
	
	
	
	/* check every case: */
	/* command */
	if (wordType == wordIsCOMMAND)
	{
		status = check_commandOperands(&line, word);
		if (status == ERROR)
			return ERROR;
		return 0;
	}
	
	/* data */
	else if (wordType == wordIsDATA)
	{
		status = check_dataValues(&line, NULL);
		if (status == ERROR)
			return ERROR;
		return 0;
	}
	
	/* string */
	else if (wordType == wordIsSTRING)
	{
		status = check_stringData(&line);
		if (status == ERROR)
			return ERROR;
		return 0;
	}
	
	/* mat */
	else if (wordType == wordIsMAT)
	{
		capacity = matHeight * matLength;
		status = check_matValues(&line, capacity);
		if (status == ERROR)
			return ERROR;
		return 0;
	}
	
	
	return 0; /* default- success */
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
	
	if (strcmp(ptr, ".data") == 0)
	{
		printf("\nLabel name cannot be \".data\". (Line %d)\n\n", lineCounter);
		return ERROR;
	}
	
	if (strcmp(ptr, ".string") == 0)
	{
		printf("\nLabel name cannot be \".string\". (Line %d)\n\n", lineCounter);
		return ERROR;
	}
	
	if (strcmp(ptr, ".mata") == 0)
	{
		printf("\nLabel name cannot be \".mata\". (Line %d)\n\n", lineCounter);
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



/* check that the label that was .entry-d wasn't also .extern-d in the same file */
int check_entryNotAlsoExterned(char *label)
{
	if (isAlreadyExtern(label))
	{
		printf("Label \"%s\" cannot be both .entry and .extern in the same file (Line %d, file %d)\n", label, lineCounter, fileCounter);
		return ERROR;
	}
	
	return 0;
}


/* check that the label that was .extern-d wasn't also .entry-d in the same file */
int check_externNotAlsoEntryed(char *label)
{
	if (isAlreadyEntry(label))
	{
		printf("Label \"%s\" cannot be both .entry and .extern in the same file (Line %d, file %d)\n", label, lineCounter, fileCounter);
		return ERROR;
	}
	
	return 0;
}

/* check that there is no garbage text before the line, and that the first word is legal and valid */
int check_garbageTextBeforeLine(char *line, int *matHeight, int *matLength)
{
	int charsRead = 0, i, len;
	char word[MAX_LINE_LENGTH], *c;
	c = line;
	
	/* skip white spaces before line */
	while (*c == ' ' || *c == '\t')
		c++;
	if (*c == '\0')
		return EMPTY_LINE;

	/* scan first word */
	if (sscanf(c, "%s%n", word, &charsRead) < 1)
		return ERROR;

	/* is this a label? (does this end with ':') */
	len = strlen(word);
	if (len == 1 && word[0] == ':')
	{
		printf("\nEmpty label decleration (Line %d, file %d)\n", lineCounter, fileCounter);
		return ERROR;
	}
	if (len > 1 && word[len-1] == ':')
		return wordIsLABEL;

	/* is this .entry? */
	if (strncmp(word, ".entry", 6) == 0)
	{
		if (len > 6) /* word is longer than '.entry\0'- there are extraneous chars sticked to it */
		{
			printf("\nExtraneous text after '.entry'. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		return wordIsENTRY;
	}
		
	/* is this .extern? */
	if (strncmp(word, ".extern", 7) == 0)
	{
		if (len > 7) /* word is longer than '.extern\0'- there are extraneous chars sticked to it */
		{
			printf("\nExtraneous text after '.extern'. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		return wordIsEXTERN;
	}

	/* is this .data? */
	if (strncmp(word, ".data", 5) == 0)
	{
		if (len > 5) /* word is longer than '.data\0'- there are extraneous chars sticked to it */
		{
			printf("\nExtraneous text after '.data'. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		return wordIsDATA;
	}
	
	
	/* is this .string? */
	if (strncmp(word, ".string", 7) == 0)
	{
		if (len > 7) /* word is longer than '.string\0'- there are extraneous chars sticked to it */
		{
			printf("\nExtraneous text after '.string'. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		return wordIsSTRING;
	}
	
	
	/* is this .mat[][]? */
	if (strncmp(word, ".mat[", 5) == 0)
	{
		c = word + 5;
		charsRead = 0;
		if (sscanf(c, "%d%n", matHeight, &charsRead) == 0)
		{
			printf("\nMissing mat height in '.mat[][]' decleration or non-int value. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		c += charsRead;
		
		if (*(c++) != ']' || *(c++) != '[')
		{
			printf("\nExtraneous text or missing bracket in '.mat' decleration. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		charsRead += 2;
		
		if (sscanf(c, "%d%n", matLength, &charsRead) == 0)
		{
			printf("\nMissing mat length in '.mat[][]' decleration or non-int value. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		c += charsRead;
		
		if (*(c++) != ']')
		{
			printf("\nExtraneous text or missing bracket in '.mat' decleration. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		charsRead++;
		
		if (*c != ' ' && *c != '\t' && *c != '\n' && *c != '\0') /* no white space or end of line after .mat decleration */
		{
			printf("\nExtraneous text after '.mat'. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		return wordIsMAT;
	}

	/* is this a command? */
	for (i = 0; i < num_of_opcodes; ++i)
		if (strcmp(word, opcodeTable[i].name) == 0)
			return wordIsCOMMAND;


	/* no legal opening was matched- this is garbage text */
	printf("\nExtraneous text before line. (Line %d)\n\n", lineCounter);
	return ERROR;
}






/* check that there is no garbage text before next word, and that the new word is legal and valid */
int check_garbageTextAndClassifyWord(char *line, int firstWord, int *matHeight, int *matLength)
{
	int charsRead = 0, i, len;
	char word[MAX_LINE_LENGTH], *c = line;
	
	/* skip white spaces before line */
	while (*c == ' ' || *c == '\t')
	{
		c++;
		charsRead++;
	}
	if (*c == '\0')
		return EMPTY_LINE;

	/* scan next word */
	if (sscanf(c, "%s%n", word, &charsRead) < 1)
		return ERROR;


	len = strlen(word);
	

	/* is this .entry? */
	if (strncmp(word, ".entry", 6) == 0) /* entry is not allowed to be anything but the first word */
	{
		printf("\n'.entry' is not allowed here. (Line %d)\n\n", lineCounter);
		return ERROR;
	}
		
	/* is this .extern? */
	if (strncmp(word, ".extern", 7) == 0) /* extern is not allowed to be anything but the first word */
	{
		printf("\n'.extern' is not allowed here. (Line %d)\n\n", lineCounter);
		return ERROR;
	}

	/* is this .data? */
	if (strncmp(word, ".data", 5) == 0)
	{
		if (firstWord != wordIsLABEL) /* only case when .data is allowed as second word is after label decleration */
		{
			printf("\n'.data' is not allowed here. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		if (len > 5) /* word is longer than '.data\0'- there are extraneous chars sticked to it */
		{
			printf("\nExtraneous text after '.data'. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		return wordIsDATA;
	}
	
	
	/* is this .string? */
	if (strncmp(word, ".string", 7) == 0)
	{
		if (firstWord != wordIsLABEL) /* only case when .string is allowed as second word is after label decleration */
		{
			printf("\n'.string' is not allowed here. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		if (len > 7) /* word is longer than '.string\0'- there are extraneous chars sticked to it */
		{
			printf("\nExtraneous text after '.string'. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		return wordIsSTRING;
	}
	
	
	/* is this .mat[][]? */
	if (strncmp(word, ".mat[", 5) == 0)
	{
		if (firstWord != wordIsLABEL) /* only case when .mat is allowed as second word is after label decleration */
		{
			printf("\n'.mat' is not allowed here. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		c = word + 5;
		charsRead = 0;
		if (sscanf(c, "%d%n", matHeight, &charsRead) == 0)
		{
			printf("\nMissing mat height in '.mat[][]' decleration or non-int value. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		c += charsRead;
		
		if (*(c++) != ']' || *(c++) != '[')
		{
			printf("\nExtraneous text or missing bracket in '.mat' decleration. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		charsRead += 2;
		
		if (sscanf(c, "%d%n", matLength, &charsRead) == 0)
		{
			printf("\nMissing mat length in '.mat[][]' decleration or non-int value. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		c += charsRead;
		
		if (*(c++) != ']')
		{
			printf("\nExtraneous text or missing bracket in '.mat' decleration. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		charsRead++;
		
		if (*c != ' ' && *c != '\t' && *c != '\n' && *c != '\0') /* no white space or end of line after .mat decleration */
		{
			printf("\nExtraneous text after '.mat'. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		return wordIsMAT;
	}

	/* is this a command? */
	for (i = 0; i < num_of_opcodes; ++i)
		if (strcmp(word, opcodeTable[i].name) == 0)
		{
			if (firstWord != wordIsLABEL) /* only case when command is allowed as second word is after label */
			{
				printf("\nCommand is not allowed here. (Line %d)\n\n", lineCounter);
				return ERROR;
			}
			
			return wordIsCOMMAND;
		}	
		
	
	
	/* is this a label? (comes after entry or extern) */
	if (firstWord == wordIsENTRY || firstWord == wordIsEXTERN) /* only two cases when label is allowed as second word is after entry or extern */
	{
		return wordIsLABEL;
	}
	
	
	/* no legal opening was matched- this is garbage text */
	printf("\nExtraneous text \"%s\". (Line %d)\n\n", word, lineCounter);
	return ERROR;
}


int check_scanOperand(char **line, int *addrMode, char *labelForCaller)
{
	int haveLabel, val, copyLen;
	char *c, *temp, labelBuf[buffer_size];
	
	
	c = *line;
	c = skipWhiteSpace(c);
	
	if (*c == '\0' || *c == '\n') /* no opernad found */
		return 0;

	/* immediate: #int */
	if (*c == '#')
	{
		temp = c + 1;
		if (!scanInt(&temp, &val))
		{
			printf("\nIllegal immediate value after '#'. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		/* after the number there must be end/space/comma/']' */
		if (*temp != '\0' && *temp != '\n' && *temp != ' ' && *temp != '\t' && *temp != ',' && *temp != ']')
		{
			printf("\nExtraneous text after immediate operand. (Line %d)\n\n", lineCounter);
			return ERROR;
		}

		*addrMode = ADDR_IMMEDIATE;
		
		if (val > max_immediate_int || val < min_immediate_int)
		{
			printf("\nNumber is out of legal range. (Line %d)\n\n", lineCounter);
			return ERROR;
		}
		
		*line = temp;
		return 1;
	}

	/* register: r0-r7 */
	if (c[0] == 'r' && c[1] >= '0' && c[1] <= '7' && (c[2] == '\0' || c[2] == '\n' || c[2] == ' ' || c[2] == '\t' || c[2] == ',' || c[2] == ']'))
	{
		*addrMode = ADDR_REGISTER;
		
		*line = c + 2;
		return 1;
	}

	/* label */
	temp = c;
	haveLabel = scanLabel(&temp, labelBuf);
	
	if (haveLabel)
	{
		if (labelForCaller && MAX_LABEL_LENGTH > 0)
		{
			copyLen = strlen(labelBuf);
			if (copyLen >= MAX_LABEL_LENGTH)
				copyLen = MAX_LABEL_LENGTH - 1;
			
			memcpy(labelForCaller, labelBuf, copyLen);
			labelForCaller[copyLen] = '\0';
		}

		/* check for mat label: LABEL[][] */
		temp = skipWhiteSpace(temp);
		
		if (*temp == '[')
		{
			/* first index- register r0-r7 */
			temp++; /* skip '[' */
			temp = skipWhiteSpace(temp);

			if (temp[0] == 'r' && temp[1] >= '0' && temp[1] <= '7' && (temp[2] == ']' || temp[2] == ' ' || temp[2] == '\t'))
			{
				temp += 2; /* skip register name */
				temp = skipWhiteSpace(temp);
				if (*temp != ']')
				{
					printf("\nMissing ']' after first matrix index. (Line %d)\n\n", lineCounter);
					return ERROR;
				}
			
				temp++; /* skip ']' */
			}
			
			else
			{
				printf("\nIllegal first matrix index (need r0-r7). (Line %d)\n\n", lineCounter);
                return ERROR;
			}
					

			/* check for second bracket */
			temp = skipWhiteSpace(temp);
			if (*temp != '[')
			{
				printf("\nMissing second '[' before second matrix index. (Line %d)\n\n", lineCounter);
				return ERROR;
			}
		
			temp++; /* skip '[' */
			temp = skipWhiteSpace(temp);

			if (temp[0] == 'r' && temp[1] >= '0' && temp[1] <= '7' && (temp[2] == ']' || temp[2] == ' ' || temp[2] == '\t'))
			{
				temp += 2; /* skip register name */
				temp = skipWhiteSpace(temp);
				if (*temp != ']')
				{
					printf("\nMissing ']' after second matrix index. (Line %d)\n\n", lineCounter);
					return ERROR;
				}
				temp++; /* skip ']' */
			}
		
			else
			{
				printf("\nIllegal second matrix index (need r0-r7). (Line %d)\n\n", lineCounter);
                return ERROR;
			}
		
			/* valid indexed operand */
			*addrMode = ADDR_INDEX;
			*line = temp;
			return 1;
		}

		/* non-mat label (no [][]) */
		*addrMode = ADDR_DIRECT;
		*line = temp;
		return 1;
	}

	/* nothing matched: no operand here */
	return 0;
}	


int check_dataValues(char **line, int *valueCount)
{
	int num, status;
	char *c;
	
	c = *line;
	
	/* skip leading spaces before first int */
	c = skipWhiteSpace(c);
	
	/* Require the first integer (empty list is illegal for .data) */
	status = scanInt(&c, &num);
	if (!status) /* valueCount == NULL means we didn't call the func for a mat[][] check */
	{
		printf("\nMissing value after '.data' decleration. (Line %d)\n\n", lineCounter);
        return ERROR;
	}
	
	if (valueCount != NULL)
		*valueCount  = 1;

	/* take in: ", int" until no more or error */
	while(1)
	{
		c = skipWhiteSpace(c);

		/* there is no comma- no more values to scan */
		if (!isRequiredComma(&c))
			break;

		c = skipWhiteSpace(c);

		/* scan next int */
		status = scanInt(&c, &num);
		if (!status) /* char after , was non-number or missing */
		{
			printf("\nMissing value/non-number after comma in a '.data' line. (Line %d)\n\n", lineCounter);
		    return ERROR;
		}
		
		if (valueCount != NULL)
			(*valueCount)++;
	}

	/* after end of ints allow only trailing white spaces */
	c = skipWhiteSpace(c);
	if (!isEndOfLine_or_whiteSpaceOnly(c))
	{
		printf("\nExtraneous text after line. (Line %d)\n\n", lineCounter);
		return ERROR;
	}

	/* everything was valid and legal- move the caller's line pointer */
	*line = c;
	return 1;
}


/* check proper comma use and number of values within the capacity of the mat (rows * columns) */
int check_matValues(char **line, int capacity)
{
	int valueCount, status;
	char *c;
	
	c = *line;
	c = skipWhiteSpace(c);
	
	/* no values is allowed for .mat - legal */
	if (isEndOfLine_or_whiteSpaceOnly(c))
	{
		*line = c;
		return 1;
	}
	
	/* use the same check as .data but also counts values and check that's within capacity */
	status = check_dataValues(&c, &valueCount);
	if (status == ERROR)
		return ERROR;
	
	if (valueCount > capacity)
	{
		printf("\nToo many values for '.mat'- max is %d. (Line %d)\n\n", capacity, lineCounter);
		return ERROR;
	}
	
	*line = c;
	return 1;
}


/* checks that the string after .string is legal (proper use of ", no extraneous text after) */
int check_stringData(char **line)
{
	int status;
	char *c, buffer[buffer_size];
	
	c = *line;
	
	/* caller already verified a space after .string - skip it */
	c = skipWhiteSpace(c);
	
	status = scanString(&c, buffer);
	if (!status)
	{
		printf("\nMissing/illegal string after '.string'. (Line %d)\n\n", lineCounter);
		return ERROR;
	}
	
	/* only white spaces are allowed after the string */
	c = skipWhiteSpace(c);
	if (!isEndOfLine_or_whiteSpaceOnly(c))
	{
		printf("\nExtraneous text after line. (Line %d)\n\n", lineCounter);
		return ERROR;
	}

	*line = c;
	return 1;
}



/* check if the adderssing methods are legal for the command */
int check_commandOperands(char **line, char *commandName)
{
	opcd *op = NULL;
	int i, needSrc, needDst, srcMode, dstMode, haveSrc, haveDst, status;
	char *c, tempSourceLabel[MAX_LABEL_LENGTH], tempDestLabel[MAX_LABEL_LENGTH];
	
	/* find opcode by name */
	for (i = 0; i < num_of_opcodes; ++i)
	{
		if (strcmp(opcodeTable[i].name, commandName) == 0)
		{
			op = &opcodeTable[i];
			break;
		}
	}
	
	if (op == NULL)
	{
		printf("\nUnknown opcode '%s'. (Line %d)\n\n", commandName, lineCounter);
		return ERROR;
	}
	
	/* check if this command expects any source operand */
	if (op->source[ADDR_IMMEDIATE] || op->source[ADDR_DIRECT] || op->source[ADDR_INDEX] || op->source[ADDR_REGISTER])
		needSrc = 1;
	else
		needSrc = 0;
	
	/* check if this command expects any dest operand */
	if (op->dest[ADDR_IMMEDIATE] || op->dest[ADDR_DIRECT] || op->dest[ADDR_INDEX] || op->dest[ADDR_REGISTER])
		needDst = 1;
	else
		needDst = 0;
	
	c = *line;
	c = skipWhiteSpace(c);
	
	haveSrc = 0;
	haveDst = 0;
	srcMode = -1;
	dstMode = -1;

	if (needSrc && needDst) /* command expects source operand and dest oprerand */
	{
		status = check_scanOperand(&c, &srcMode, tempSourceLabel);
		if (!status)
		{
			printf("\nMissing/illegal source operand for '%s'. (Line %d)\n\n", commandName, lineCounter);
			return ERROR;
		}
		haveSrc = 1; /* mark success */

		c = skipWhiteSpace(c);
		if (!isRequiredComma(&c))
		{
			printf("\nMissing comma between operands for '%s'. (Line %d)\n\n", commandName, lineCounter);
			return ERROR;
		}
		c = skipWhiteSpace(c);

		status = check_scanOperand(&c, &dstMode, tempDestLabel);
		if (!status)
		{
			printf("\nMissing/illegal destination operand for '%s'. (Line %d)\n\n", commandName, lineCounter);
			return ERROR;
		}
		haveDst = 1; /* mark success */
	}
	
	else if (!needSrc && needDst) /* command expects only dest oprerand */
	{
		status = check_scanOperand(&c, &dstMode, tempDestLabel);
		if (!status)
		{
			printf("\nMissing/illegal destination operand for '%s'. (Line %d)\n\n", commandName, lineCounter);
			return ERROR;
		}
		haveDst = 1; /* mark success */

		c = skipWhiteSpace(c);
		if (isRequiredComma(&c))
		{
			printf("\nExtraneous comma in single-operand command '%s'. (Line %d)\n\n", commandName, lineCounter);
			return ERROR;
		}
	}
	
	else /* command expects no oprerands */
	{
		if (!isEndOfLine_or_whiteSpaceOnly(c))
		{
			printf("\nExtraneous text after zero-operand command '%s'. (Line %d)\n\n", commandName, lineCounter);
			return ERROR;
		}
	}
	
	/* trailing junk check (for the commands that need 1 or 2 operands) */
	c = skipWhiteSpace(c);
	if (!isEndOfLine_or_whiteSpaceOnly(c))
	{
		printf("\nExtraneous text after line. (Line %d)\n\n", lineCounter);
		return ERROR;
	}
	
	/* addressing mode validation via opcode table */
	if (haveSrc)
		if (srcMode < 0 || srcMode > 3 || !op->source[srcMode])
		{
			printf("\nIllegal addressing mode for source operand in command '%s'. (Line %d)\n\n", commandName, lineCounter);
			return ERROR;
		}
	
	if (haveDst)
		if (dstMode < 0 || dstMode > 3 || !op->dest[dstMode])
		{
			printf("\nIllegal addressing mode for destination operand in command '%s'. (Line %d)\n\n", commandName, lineCounter);
			return ERROR;
		}
	
	*line = c; /* move caller's line pointer */
	return 1;
}


















/* TEMP- this is Tomer's */
int isLabel(char *ptr) {
    int len = 0;
    while (ptr[len] && !isspace((unsigned char)ptr[len]) && ptr[len] != ':') len++;
    
    if (ptr[len] == ':' && check_labelName(ptr) == ERROR)
        return ERROR;
    
    return ptr[len] == ':';
}

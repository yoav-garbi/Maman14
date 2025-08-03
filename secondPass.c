#include "prototypes.h"



int secondPass(int argc, char *argv[], FILE **fileArr, lineNode *lineArr[], char **nameArr)
{
	int fileCount, errorFlag = 0;
	char *character, label[buffer_size], binAddress[address_binary_representation_size];
	binTree *node;
	
	/* replace lebels with address */
	for (fileCount = 0; fileCount < argc-1; ++fileCount) /* each iteration is one file */
	{
		for (lineCounter = 0; lineArr[lineCounter] != NULL; lineCounter++) /* each iteration is one line */
		{
			for (character = lineArr[lineCounter]->line; *character != '\n'; character++) /* start of line to end of line, each iteration is one char */
			{
				while (*character != ' ' && *character != '\n')
					character++;
				
				if (*character == '\n')
					break;
				
				sscanf(character, "%s", label);
				node = search(labelTable, label);
				
				if (check_labelExist(node) == ERROR)
				{
					errorFlag = 1;
					break; /* go to next line */
				}
				
				base10_to_base2_forAddress(node->address, binAddress); /* translate address to binary */
				
				sprintf(character, "%s", binAddress); /* use address to overwrite the label */
				
				while (*character != '\0') /* go to end of address */
					character++;
				
				while (*character != ' ' && *character != '\n')	/* delete any remaining parts of the label */
					*character = ' ';
				
				/* continue scanning from current character for more labels */
			}
		}
		
		
		/* copy lines to file */
		for (lineCounter = 0; lineArr[lineCounter] != NULL; lineCounter++) /* each iteration is one line */
		{
			
			/*TODO- create .ob file*/
				
			for (character = lineArr[lineCounter]->line; *character != '\0'; character++) /* start of line to end of line, each iteration is one char */
			{
				if (*character == ' ')
					continue;
				
				/*fputc(*character, fileArr[2*(argc-1) + fileCount]);															FIX*/
			}
		}
	}
	
	
	
	/* create extension files (if there were no errors) */
	if (errorFlag == 1)
		return ERROR;
	
	/*makeOutFiles(argc, argv, fileArr, nameArr);																REMOVE*/
	
	for (fileCount = 0; fileCount < argc-1; ++fileCount) /* each iteration is one file */
	{
		if (searchEnt(labelTable))	/* if there is an entry to put in .ent file */
		{
			create_entFile(argc, fileArr, nameArr, fileCount);
			writeEnt(fileArr[3*(argc-1) + fileCount], labelTable);
		}
		
		if (searchExt(labelTable))	/* if there is an extern label to put in .ext file */
		{
			create_extFile(argc, fileArr, nameArr, fileCount);
			writeExt(fileArr[4*(argc-1) + fileCount], labelTable);
		}
	}
	
	return 0;
}

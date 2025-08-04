#include "prototypes.h"



int secondPass(int argc, char *argv[], FILE **fileArr, lineNode *lineArr[], char **nameArr)
{
	int fileCount, errorFlag = 0;
	char *character, label[buffer_size], binAddress[address_binary_representation_size];
	binTree *node;
	lineNode *line;
	FILE *tempFile;
	
	/* replace lebels with address */
	for (fileCount = 0; fileCount < argc-1; ++fileCount) /* each iteration is one file */
	{
		if (lineArr[fileCount] == NULL)
			break;
		
		for (line = lineArr[fileCount], lineCounter = 0; line != NULL; line = line->next, lineCounter++) /* each iteration is one line */
		{
			character = line->line;
			
			if (*character != ' ') /* if there's a label in the line, it is always the first thing in the line */
				continue;
			
			if (*character == '\n')
				continue;
			
			sscanf(character, "%s", label);
			node = search(labelTable, label);
			
			if (check_labelExist(node) == ERROR)	/* if label isn't in labelTable */
			{
				errorFlag = 1;
				continue; /* go to next line */
			}
			
			base10_to_base2_forAddress(node->address, binAddress); /* translate address to binary */
			
			memcpy(character + 1, binAddress, 8); /* use address to overwrite the label */
			
			while (*character != ' ')	/* delete any remaining parts of the label */
				*character = ' ';
		}

		
		
		/* copy lines to file */
		if (errorFlag == 1)
			return ERROR;
		
		for (fileCount = 0; lineArr[fileCount] != NULL; fileCount++) /* each iteration is one file */
		{
			create_obFile(argc, fileArr, nameArr, fileCount);
			
			
			for (line = lineArr[fileCount]; line != NULL; line = line->next) /* each iteration is one line */
				for (character = line->line; *character != '\0'; character++) /* each iteration is one char */
				{
					if (*character == ' ')
						continue;
				
					fputc(*character, fileArr[2*(argc-1) + fileCount]);
				}
			
			tempFile = fopen("temp", "w+");
			if (check_newFileExistence(tempFile) == ERROR)
				return ERROR;
				
			copyFile(fileArr[2*(argc-1) + fileCount], tempFile);
			freopen(NULL, "w+", fileArr[2*(argc-1) + fileCount]); /* truncate and reopen the ob file */
			
			base2_to_base4_fileToFile(tempFile, fileArr[2*(argc-1) + fileCount]);
			
			fclose(tempFile);
			remove("temp");
		}
	}
	
	
	
	/* create extension files (if there were no errors) */
	if (errorFlag == 1)
		return ERROR;
	
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

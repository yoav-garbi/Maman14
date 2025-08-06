#include "prototypes.h"



int secondPass(int argc, char *argv[], FILE **fileArr, lineNode *lineArr[], char **nameArr)
{
	int errorFlag = 0, numFiles = argc - 1, obOffset = 2 * numFiles, entOffset = 3 * numFiles, extOffset = 4 * numFiles;
	char *character, label[buffer_size], binAddress[address_binary_representation_size];
	binTree *node;
	lineNode *line;
	FILE *tempFile;
	
	/* replace lebels with address */
	for (fileCounter = 0; fileCounter < numFiles; ++fileCounter) /* each iteration is one file */
	{
		if (lineArr[fileCounter] == NULL)
			break;
		
		for (line = lineArr[fileCounter], lineCounter = 0; line != NULL; line = line->next) /* each iteration is one line */
		{
			character = line->line;
			
			if (*character != ' ') /* if there's a label in the line, it is always the first thing in the line */
				continue;
			
			if (*character == '\n')
				continue;
			
			sscanf(character, "%s", label);
			node = search(labelTable[fileCounter], label);
			
			if (check_labelExist_or_legalExternalUse(node, label, line, numFiles) == ERROR) /* if label is ilegaly external or isn't in labelTable at all */
			{
				errorFlag = 1;
				continue; /* go to next line */
			}
			
			base10_to_base2_forAddress(node->address, binAddress); /* translate address to binary */
			
			memcpy(character + 1, binAddress, 8); /* use address to overwrite the label */
			
			while (*character != ' ')	/* delete any remaining parts of the label */
				*character = ' ';
		}
	}


		
	if (errorFlag == 1)
		return ERROR;
	
	
	
	/* create .ob file (copy lines to file and translate to base4) */
	for (fileCounter = 0; fileCounter < numFiles && lineArr[fileCounter] != NULL; fileCounter++) /* each iteration is one file */
	{
		create_obFile(argc, fileArr, nameArr, fileCounter);
		
		
		for (line = lineArr[fileCounter]; line != NULL; line = line->next) /* each iteration is one line */
			for (character = line->line; *character != '\0'; character++) /* each iteration is one char */
			{
				if (*character == ' ')
					continue;
			
				fputc(*character, fileArr[obOffset + fileCounter]);
			}
		
		tempFile = fopen("temp", "w+");
		if (check_newFileExistence(tempFile) == ERROR)
			return ERROR;
			
		copyFile(fileArr[obOffset + fileCounter], tempFile);
		freopen(NULL, "w+", fileArr[obOffset + fileCounter]); /* truncate and reopen the ob file */
		
		base2_to_base4_fileToFile(tempFile, fileArr[obOffset + fileCounter]);
		
		fclose(tempFile);
		remove("temp");
	}
	
	
	
	
	/* create extension files (if there were no errors) */
	for (fileCounter = 0; fileCounter < numFiles; ++fileCounter) /* each iteration is one file */
	{
		if (searchEnt(labelTable[fileCounter]))	/* if there is an entry to put in .ent file */
		{
			create_entFile(argc, fileArr, nameArr, fileCounter);
			writeEnt(fileArr[entOffset + fileCounter], labelTable[fileCounter]);
		}
		
		if (searchExt(labelTable[fileCounter]))	/* if there is an extern label to put in .ext file */
		{
			create_extFile(argc, fileArr, nameArr, fileCounter);
			writeExt(fileArr[extOffset + fileCounter], labelTable[fileCounter]);
		}
	}
	
	return 0;
}

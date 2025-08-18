#include "prototypes.h"



int secondPass(int argc, char *argv[], FILE **fileArr, lineNode *lineArr[], char **nameArr)
{
	int errorFlag = 0, numFiles = argc - 1, obOffset = 2 * numFiles, entOffset = 3 * numFiles, extOffset = 4 * numFiles, type;
	char *character, label[buffer_size], *labelPtr, binAddress[address_binary_representation_size+3];
	binTree *node;
	lineNode *line, *entryLine, *externLine;
	FILE *tempFile;
	
	
	/* add/update enty labels across all labelTables */
	for (fileCounter = 0; fileCounter < numFiles; ++fileCounter) /* each iteration deals with one file */
		for (entryLine = entryLineArr[fileCounter]; entryLine != NULL; entryLine = entryLine->next) /* each iteration deals with one label */
		{
			lineCounter = entryLine->lineNum;
			labelPtr = entryLine->line;
			
			/* mark this label's real definition as an entry */
			if (addEntryLocal(labelPtr) == ERROR || check_entryDeclaredInOtherFile(labelPtr, numFiles) == ERROR)
			{
				errorFlag = 1;
				continue; /* skip importing undefined or duplicate entry */
			}
			
			/* import the label into other files as an external label */
			node = search(labelTable[fileCounter], labelPtr);
			if (node != NULL)
			{
				entryLine->address = node->address; /* store real address for .ent file */
				addExternAcross(entryLine->line, node->symbolType, numFiles);
			}
		}
	
	/* mark existing nodes as external or insert a new node with address=0 */
	for (fileCounter = 0; fileCounter < numFiles; ++fileCounter) /* each iteration deals with one file */
		for (externLine = externLineArr[fileCounter]; externLine != NULL; externLine = externLine->next) /* each iteration deals with one label */
		{ 
			lineCounter = externLine->lineNum;
			labelPtr = externLine->line;
			node = search(labelTable[fileCounter], labelPtr);
			
			if (node != NULL) /* there is already an existing node- mark it as external */
			{
				if (node->symbolType == EXTERN)
				{
					if (check_isExternalLabelDefinedInOtherFile(labelPtr, numFiles) == ERROR)
						errorFlag = 1;
				}
				node->isExternal = 1;
			}
				
			else /* there is no symbol yet- insert an external label node (address=0) */
			{
				type = check_isExternalLabelDefinedInOtherFile(labelPtr, numFiles);
				
				if (type != ERROR)
					addNode(&labelTable[fileCounter], labelPtr, 0, CODE, 1, 0);
				else
					errorFlag = 1;
			}
		}
	
	
	/* replace labels with address */
	for (fileCounter = 0; fileCounter < numFiles; ++fileCounter) /* each iteration is one file */
	{
		if (lineArr[fileCounter] == NULL)
			break;
		
		for (line = lineArr[fileCounter], lineCounter = 0; line != NULL; line = line->next, ++lineCounter) /* each iteration is one line */
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
			
			/* overwrite placeholder with address bits and ERA */
			if (node->isExternal)
			{
				binAddress[address_binary_representation_size - 2] = '0';
				binAddress[address_binary_representation_size - 1] = '1';
			}
			else
			{
				binAddress[address_binary_representation_size - 2] = '1';
				binAddress[address_binary_representation_size - 1] = '0';
			}
			
			memcpy(character + 1, binAddress, address_binary_representation_size);
			character[1 + address_binary_representation_size] = '\0';
		}
	}


		
	if (errorFlag == 1)
		return ERROR;
	
	
	
	/* create .ob file (copy lines to file and translate to base4) */
	for (fileCounter = 0; fileCounter < numFiles && lineArr[fileCounter] != NULL; fileCounter++) /* each iteration is one file */
	{
		create_obFile(argc, fileArr, nameArr, fileCounter);
		
		/* write IC and DC in first line */
		base10_to_base2(icArr[fileCounter] - IC_INIT_VALUE, binAddress);
		fprintf(fileArr[obOffset + fileCounter], "\t %s ", binAddress);
		base10_to_base2(dcArr[fileCounter], binAddress);
		fprintf(fileArr[obOffset + fileCounter], "%s\n", binAddress);
		
		
		for (line = lineArr[fileCounter]; line != NULL; line = line->next) /* each iteration is one line */
		{
			base10_to_base2_forShortAddress(line->address, binAddress);
			fprintf(fileArr[obOffset + fileCounter], "%s\t\t", binAddress);
			
			for (character = line->line; *character != '\0'; character++) /* each iteration is one char */
			{
				if (*character == ' ' || *character == '\t')
					continue;
			
				fputc(*character, fileArr[obOffset + fileCounter]);
			}
			
			fprintf(fileArr[obOffset + fileCounter], "\n");
		}
		
		tempFile = fopen("temp", "w+");
		if (check_newFileExistence(tempFile) == ERROR)
		return ERROR;

		copyFile(fileArr[obOffset + fileCounter], tempFile);
		fclose(fileArr[obOffset + fileCounter]);
		fileArr[obOffset + fileCounter] = fopen(nameArr[fileCounter], "w+"); /* reopen and truncate the ob file */
		if (check_newFileExistence(fileArr[obOffset + fileCounter]) == ERROR)
		{
			fclose(tempFile);
			remove("temp");
			return ERROR;
		}

		base2_to_base4_fileToFile(tempFile, fileArr[obOffset + fileCounter]);

		fclose(tempFile);
		remove("temp");
	}
	
	
	
	
	/* create extension files (if there were no errors) */
	for (fileCounter = 0; fileCounter < numFiles; ++fileCounter) /* each iteration is one file */
	{
		if (entryLineArr[fileCounter] != NULL)	/* if there is an entry to put in .ent file */
		{
			create_entFile(argc, fileArr, nameArr, fileCounter);
			writeEnt(fileArr[entOffset + fileCounter]);
		}
		
		for (externLine = externLineArr[fileCounter]; externLine != NULL && externLine->address == 0; externLine = externLine->next); /* look for extern usage */
		if (externLine != NULL) /* create .ext file only if a usage was found */
		{
			create_extFile(argc, fileArr, nameArr, fileCounter);
			writeExt(fileArr[extOffset + fileCounter]);
		}
	}
	
	return 0;
}

#include "prototypes.h"

extern binTree *labelTable;


int secondPass(int argc, FILE **fileArr, lineNode *lineArr[])
{
	int fileCount /*, status*/;
	char /* *line,*/ *character, label[buffer_size], binAddress[binary_representation_size];
	binTree *node;	
	
	
	for (fileCount = 0; argc-1; ++fileCount) /* each iteration is one file */
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
					break; /* go to next line */
				
				base10_to_base2_forAddress(node->address, binAddress); /* translate address to binary */
				
				sprintf(character, "%s", binAddress); /* use address to overwrite the label */
				
				while (*character != '\0') /* go to end of address */
					character++;
				
				while (*character != ' ' && *character != '\n')	/* delete any remaining parts of the label */
					*character = ' ';
				
				/* continue scanning from current character for more labels */
			}
		
			
			/* TODO- if EOF- exit loop*/
		
			/* TODO- complete binary coding by replacing the labels with the address */
		
		}
		
		
		/* TODO- output files (if no errors) */
		
		
		
	}
	
	
	
	return 0;
}

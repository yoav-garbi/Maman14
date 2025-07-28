#include "prototypes.h"
#include "header.h"



/* check if a file with a given name exists in the directory */
int check_fileExistence(void *pointer)
{
	if (pointer == NULL)
	{
		fprintf(stderr, "\nSource file does not exist.\n\n");
		return ERROR;
	}
	
	return 0;
}


/* check if a command in the source code was mis-written */
int check_opcodeName(int index)
{
	if (index < num_of_opcodes) /* a command was found */
		return 0;
	
	fprintf(stderr, "\nUnknown command name.\n\n");
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
	
	fprintf(stderr, "\nIlegal addressing method for this command.\n\n");
	return ERROR;
}

#include "prototypes.h"
#include "header.h"

int lineCounter;


/* check if a file with a given name exists in the directory */
int check_fileExistence(void *pointer)
{
	if (pointer == NULL)
	{
		printf("\nSource file does not exist. (Line %d)\n\n", lineCounter);
		return ERROR;
	}
	
	return 0;
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
	int status = strlen(buffer) <= 82; /* 80 chars + \n + \0 */
	
	if (status)
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
	int status = strlen(buffer) <= 31; /* 30 chars + \0 */
	
	if (status)
		return 0; /* this means that the label has legal length */
	
	printf("\nLabel exceeding the allowed length of 30 chars. (Line %d)\n\n", lineCounter);
	return ERROR;	
}

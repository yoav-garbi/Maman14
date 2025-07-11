#include <stdio.h>
#include <stdlib.h>
#include "prototypes.h"



int checkFileExistence(void *pointer)
{
	if (pointer == NULL)
	{
		printf("\nSource file does not exist.\n\n");
		return ERROR;
	}
	
	return 0;
}






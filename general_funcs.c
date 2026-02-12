#include "prototypes.h"


/* translates FILE in binary to FILE in base4 */
int base2_to_base4_fileToFile(FILE *source, FILE *dest)
{
	int i;
	char c1, c2, result; 
	char buffer[buffer_size];
	void *status;
	
	rewind(source);
	status = fgets(buffer, buffer_size, source);
	
	while (status != NULL) /* until end of file */
	{
		i = 0;
		
		while (buffer[i] != '\0') /* until end of line */
		{
			/* write \n */
			if (buffer[i] == '\n')
			{
				fputc('\n', dest);
				break;
			}
			
			c1 = buffer[i++];
			c2 = buffer[i++];
			
			if (c1 == '0' && c2 == '0')
				result = 'a';

			else if (c1 == '0' && c2 == '1')
				result = 'b';

			else if (c1 == '1' && c2 == '0')
				result = 'c';

			else if (c1 == '1' && c2 == '1')
				result = 'd';

			else
				result = '?'; 
			
			fputc(result, dest);
		}
		
		status = fgets(buffer, buffer_size, source);
	}
	
	
	return 0;
}

/* translates string in binary to FILE in base4 */
int base2_to_base4_strToFile(char *source, FILE *dest)
{
	int i = 0;
	char c1, c2, result; 
	
	if (source == NULL)
		return ERROR;

	while (source[i] != '\0')
	{
		/* skip and write white spaces */
		while (source[i] != '0' && source[i] != '1' && source[i] != '\0') 
		{
			fprintf(dest, "%c", source[i++]);
		}
	
		if (source[i] == '\0')
			break;
	
		c1 = source[i++];
	
	
		/* skip and write white spaces */
		while (source[i] != '0' && source[i] != '1' && source[i] != '\0') 
		{
			fprintf(dest, "%c", source[i++]);
		}
	
		if (source[i] == '\0')
			break;
	
		c2 = source[i++];
	
	
		if (c1 == '0' && c2 == '0')
			result = 'a';

		else if (c1 == '0' && c2 == '1')
			result = 'b';

		else if (c1 == '1' && c2 == '0')
			result = 'c';

		else if (c1 == '1' && c2 == '1')
			result = 'd';

		else
			result = '?'; 
	
		fputc(result, dest);
	}
	
	
	return 0;
}




int base10_to_base2(int num, char str[])
{
	int i, bitCount;
	char tempStr[buffer_size];
	
	/* translate num from decimal to binary into the temporary str (it is needed because the number comes out backwards) */
	for (i = 0; num != 0; i++)
	{
		if (num % 2)
			tempStr[i] = '1';
		else
			tempStr[i] = '0';
		
		num >>= 1;
	}
	bitCount = i;

	/* round up bitCount to nearest multiple of 4 */
	if (bitCount % 4 != 0)
		bitCount += 4 - (bitCount % 4);
	
	/* add zeros to complete to a number with a width of 4 (or a multiple op 4) */
	while (i < bitCount)
		tempStr[i++] = '0';
		
	/* copy temp into str in reverse (the correct way) */
	for (i = 0; i < bitCount; i++)
		str[i] = tempStr[bitCount - 1 - i];
	
	str[i] = '\0';
	
	return 0;
}


/* differs from  base10_to_base2 only by insuring that the number is 8 chars long (as standard for an address) */
int base10_to_base2_forAddress(int num, char str[])
{
	int i;
	char tempStr[address_binary_representation_size+1];	/* +1 for the '\0' */
	
	/* translate num from decimal to binary into the temporary str (it is needed because the number comes out backwards) */
	for (i = 0; num != 0; i++)
	{
		if (num % 2)
			tempStr[i] = '1';
		else
			tempStr[i] = '0';
		
		num >>= 1;
	}
	
	/* add zeros to complete to a number with a width of 4 (or a multiple op 4) */
	while (i < address_binary_representation_size)
		tempStr[i++] = '0';
		
	/* copy temp into str in reverse (the correct way) */
	for (i = 0; i < address_binary_representation_size; i++)
		str[i] = tempStr[address_binary_representation_size - 1 - i];
	
	str[i] = '\0';
	
	return 0;
}



int copyFile(FILE *source, FILE *dest)
{
	int c = ' ';
	
	fflush(source);
	fflush(dest);
	rewind(source);
	rewind(dest);
	
	while ((c = fgetc(source)) != EOF)
		fputc(c, dest);
	
	fflush(source);
	fflush(dest);
	rewind(source);
	rewind(dest);
	
	return 0;
}


char *strDuplicate(char *str)
{
	char *copy = malloc(strlen(str) + 1);
	if (copy)
		strcpy(copy, str);

	return copy;
}




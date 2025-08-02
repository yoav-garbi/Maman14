#include "prototypes.h"



int base2_to_base4(void *source, void *dest)
{
	int i;
	char c1, c2, result; 
	char buffer[buffer_size];
	void *status;
	
	status = fgets(buffer, buffer_size, source);
	
	while (status != NULL)
	{
		i = 0;
		
		while (buffer[i] != '\0')
		{
			/* skip and write white spaces */
			while (buffer[i] != '0' && buffer[i] != '1' && buffer[i] != '\0') 
			{
				fprintf(dest, "%c", buffer[i++]);
			}
			
			if (buffer[i] == '\0')
				break;
			
			c1 = buffer[i++];
			
			
			/* skip and write white spaces */
			while (buffer[i] != '0' && buffer[i] != '1' && buffer[i] != '\0') 
			{
				fprintf(dest, "%c", buffer[i++]);
			}
			
			if (buffer[i] == '\0')
				break;
			
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
			
			fprintf(dest, "%c", result);
		}
		
		status = fgets(buffer, buffer_size, source);
	}
	
	
	return 0;
}




int base10_to_base2(int num, char str[])
{
	int i, bitCount;
	char tempStr[binary_representation_size];
	
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
	char tempStr[binary_representation_size];
	
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
	while (i < binary_representation_size)
		tempStr[i++] = '0';
		
	/* copy temp into str in reverse (the correct way) */
	for (i = 0; i < binary_representation_size; i++)
		str[i] = tempStr[binary_representation_size - 1 - i];
	
	str[i] = '\0';
	
	return 0;
}

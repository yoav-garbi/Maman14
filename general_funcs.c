#include <stdio.h>
#include <stdlib.h>
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

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
			while (buffer[i] != '0' && buffer[i] != '1' && buffer[i] != '\0')
				fputc(buffer[i++], dest);
			
			if (buffer[i] == '\0')
				break;
			
			c1 = buffer[i++];
			
			
			while (buffer[i] != '0' && buffer[i] != '1' && buffer[i] != '\0')
				fputc(buffer[i++], dest);
			
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
			
			fputc(result, dest);
		}
		
		if (i > 0 && buffer[i-1] != '\n')
			fputc('\n', dest);
		
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
	
	while (source[i] != '\0') /* until end of line */
	{	
		while (source[i] != '0' && source[i] != '1' && source[i] != '\0')
				fputc(source[i++], dest);
			
			if (source[i] == '\0')
				break;
			
			c1 = source[i++];
			
			
			while (source[i] != '0' && source[i] != '1' && source[i] != '\0')
				fputc(source[i++], dest);
			
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
	
	/* handle zero explicitly to avoid returning an empty string */
	if (num == 0)
	{
		/* ensure at least one full pair of bits for base-4 conversion */
		str[0] = '0';
		str[1] = '0';
		str[2] = '0';
		str[3] = '0';
		str[4] = '\0';
		return 0;
	}
	
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


/* differs from base10_to_base2 by enforcing a fixed width of address_binary_representation_size characters (10 bits including ARE) */
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



int base10_to_base2_forShortAddress(int num, char str[])
{
	int i;
	char tempStr[short_address_binary_representation_size + 1]; /* +1 for '\0' */
	
	for (i = 0; num != 0; i++)
	{
		if (num % 2)
			tempStr[i] = '1';
		else
			tempStr[i] = '0';

		num >>= 1;
	}
	
	while (i < short_address_binary_representation_size)
		tempStr[i++] = '0';

	for (i = 0; i < short_address_binary_representation_size; i++)
		str[i] = tempStr[short_address_binary_representation_size - 1 - i];
	
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



char *skipWhiteSpace(char *c)
{
	while (*c == ' ' || *c == '\t')
		c++;
	return c;
}


int isEndOfLine_or_whiteSpaceOnly(char *c)
{
	c = skipWhiteSpace(c);
	return *c == '\0' || *c == '\n';
}


int isRequiredComma(char **pointer)
{
	char *c;
	c = skipWhiteSpace(*pointer);
	
	if (*c != ',')
		return 0;
	c++;
	c = skipWhiteSpace(c);
	*pointer = c;
	
	return 1;
}

/* read a label and advance pointer. return: 0 if not label, 1 if label */
int scanLabel(char **line, char *label)
{
	char *lineP = *line, *labelP = label;
	int len = 0;
	
	if (!isalpha((unsigned char)*lineP)) /* first char isn't a letter- can't be a label */
		return 0;
	
	while (isalnum((unsigned char)*lineP)) /* all non-first chars of the label can be a number too (alnum- both alphabet or numbers) */
	{
		if (len >= MAX_LABEL_LENGTH)
			return 0;
		
		*labelP++ = *lineP;
		lineP++;
		len++;
	}
	
	*labelP = '\0';
	*line = lineP; /* advance the actual line pointer (from the caller) */
	return 1;
}


/* scan for an int. int will be held in num. return: 0 if failed to detect a number, 1 if success */
int scanInt(char **line, int *num)
{
	char *c;
	int sign = 1;
	int val = 0;
	int gotDigit = 0;
	
	c = skipWhiteSpace(*line);
	
	if (*c == '+' || *c == '-')
	{
		if (*c == '-')
			sign = -1;
		
		c++;
	}
	
	while (isdigit((unsigned char)*c))
	{
		gotDigit = 1;
		val = val * 10 + (*c - '0');
		c++;
	}
	
	if (!gotDigit) /* no number was detected */
		return 0;
	
	/* check if the number is a float (containing a '.') */
	if (*c == '.')
	{
		printf("\nUse of a fraction (float)- illegal. (Line %d, file: \"%s\")\n\n", lineCounter, nameArr[fileCounter]);
		return ERROR;
	}
	
	*num = sign * val;
	*line = c; /* advance caller's pointer to after the number */
	return 1;
}


/* scan for a string that bordered by " " */
int scanString(char **line, char *str)
{
	char *start;
	char *c, *end = NULL;
	int len = 0;
	
	start = skipWhiteSpace(*line);
	
	if (isEndOfLine_or_whiteSpaceOnly(start)) /* not string data found */
	{
		printf("\nMissing string data after directive. (Line %d, file: \"%s\")\n\n", lineCounter, nameArr[fileCounter]);
		*line = start;
		return ERROR;
	}
	
	if (*start != '"') /* not starting " found */
	{
		printf("\nMissing \" before string. (Line %d, file: \"%s\")\n\n", lineCounter, nameArr[fileCounter]);
		c = start;
		while (*c && *c != '\n')
			c++;
		*line = c;
		return ERROR;
	}
	
	/* look ahead to find the closing " and count the length of the string */
	c = start + 1;
	
	while (*c && *c != '\n')
	{
		if (*c == '"')
		{
			end = c;
			break;
		}
		c++;
	}
	
	if (end == NULL) /* no closing " was found */
	{
		printf("\nMissing \" after string. (Line %d, file: \"%s\")\n\n", lineCounter, nameArr[fileCounter]);
		*line = c;
		return ERROR;
	}
	
	len = end - (start + 1);
	
	if (len >= buffer_size) /* string too long */
	{
		printf("\nString is too long. (Line %d, file: \"%s\")\n\n", lineCounter, nameArr[fileCounter]);
		return ERROR;
	}

	/* copy string and add \0 */
	if (len > 0)
		memcpy(str, start + 1, (size_t)len);
	str[len] = '\0';

	/* advance caller pointer to right after the closing " */
	*line = end + 1;
	return 1;
}


																																			/* TEMP */
int printFile(FILE *fp)
{
	int c;
	rewind(fp);
	while ((c = fgetc(fp)) != EOF)
		putchar(c);
	
	rewind(fp);
	return 0;
}

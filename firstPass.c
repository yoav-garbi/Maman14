#include "library.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "prototypes.h"

/*
 * Skips all leading spaces and tab characters in a line.
 * Returns a pointer to the first non-whitespace character.
 */
char *skipWhitespace(char *line);


/*
 * Checks if the first word in the line is a label.
 * Returns 1 if a label is found, 0 otherwise.
 */
int isLabel(const char *ptr);


char *skipWhitespace(char *line) {
    while (*line == ' ' || *line == '\t') {
        line++;
    }
    return line;
}


int isLabel(const char *ptr,binTree *lableTable) {
    int len = 0;
    while (ptr[len] && !isspace((unsigned char)ptr[len]) && ptr[len] != ':') {
        len++;
    }
    if (ptr[len] == ':' && len > 0 && len <= MAX_LABEL_LENGTH) {
        return 1;
    }
    return 0;
}

int firstPass(const char *fileName) {
    FILE *fp;
    int IC=IC_INIT_VALUE;
    int DC=DC_INIT_VALUE;
    char first_word[MAX_LABEL_LENGTH];
    int lineNumber=1;
    LineDate currentLine;
    fp=fopen(fileName, "r");
    /*maybe change for method*/
    if (fp==NULL) {
        printf("Error opening file %s\n", fileName);
    }
    currentLine.lineNumber=lineNumber;
    while (fgets(currentLine.content, MAX_LINE_LENGTH, fp)!=NULL) {
        sscanf(currentLine.content, "%s", first_word);
        if (isLabel(first_word)) {
            /*if the line starts with a lable then we would add it to the lable table*/
            /*need to use method to check if lable name is ok(len<30) and doesnt exsist*/
        }
        else if (first_word =="entry") {
            break;
        }
        else if (first_word =="external") {
            /* do instruction 10 from the algo */
        }
        /*need to check if first_word is in the op code table */
        /*need to create data base for the op code commends to save them for later8/
    }

}
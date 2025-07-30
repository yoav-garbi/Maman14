#include "firstPass.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "prototypes.h"

/*
 * Skips all leading spaces and tab characters in a line.
 * Returns a pointer to the first non-whitespace character.
 */
char *skipWhitespace(char *line) {
    while (*line == ' ' || *line == '\t') {
        line++;
    }
    return line;
}

/*
 * Checks if the first word in the line is a label.
 * Returns 1 if a label is found, 0 otherwise.
 */
int isLabel(const char *ptr) {
    int len = 0;
    while (ptr[len] && !isspace((unsigned char)ptr[len]) && ptr[len] != ':') {
        len++;
    }
    if (ptr[len] == ':' && len > 0 && len <= MAX_LABEL_LENGTH) {
        return 1;
    }
    return 0;
}

int firstPass(const char *fileName,binTree *labelTable) {
    FILE *fp;
    int IC = IC_INIT_VALUE;
    int DC = DC_INIT_VALUE;
    char first_word[MAX_LABEL_LENGTH];
    int lineNumber = 1;
    int countError = 0;
    LineDate currentLine;
    char *ptr;

    fp = fopen(fileName, "r");
    /*maybe change to a method */
    if (fp == NULL) {
        printf("Error opening file %s\n", fileName);
        return -1;
    }

    while (fgets(currentLine.content, MAX_LINE_LENGTH, fp) != NULL) {
        currentLine.lineNumber = lineNumber;
        currentLine.hasError = 0;
        currentLine.hasLabel = 0;
        currentLine.error[0] = '\0';
        currentLine.label[0] = '\0';

        ptr = skipWhitespace(currentLine.content);

        sscanf(ptr, "%s", first_word);

        if (isLabel(ptr)) {
            /*maybe change to a method */
            if (strlen(first_word) > MAX_LABEL_LENGTH) {
                countError++;
                currentLine.hasError = 1;
                strcpy(currentLine.error, "label name too long");
            } else {
                strcpy(currentLine.label, first_word);
                currentLine.hasLabel = 1;
            }
        }

        else if (strcmp(first_word, ".entry") == 0) {
            lineNumber++;
            continue;
        }

        else if (strcmp(first_word, ".extern") == 0) {
            lineNumber++;

            continue;
        }

        /* TODO: check if first_word is in the opcode table */
        /* TODO: process data, string, mat, etc */

        lineNumber++;
    }
    fclose(fp);

    if (countError > 0) {
        return countError;
    }
    return 0;
}

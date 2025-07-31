#include "firstPass.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "prototypes.h"

/*
 * Checks if the word is data (.data, .string, .mat).
 * Returns 1 if data, 0 if not.
 */
int isData(char *word);

/*
 * Checks if the word is a legal opcode (instruction).
 * Returns 1 if opcode, 0 otherwise.
 */
int isInstruction(char *word);

/*
 * Skips all spaces and tab characters in a line.
 * Returns a pointer to the first non-whitespace character.
 */
char *skipWhitespace(char *line);

/*
 * Checks if the first word in the line is a label.
 * Returns 1 if its label, 0 otherwise.
 */
int isLabel(char *ptr);

/*
 *adds a data symbol to the symbol table
 *return 1 if it was added, 0 if not
 */
int addSymbolToData(binTree **root, char *str, int address);
/*
 *returns the amount of numbers in the .data line
 */
int countDataValues(char *line);

/*
 * Counts the total memory required for a mat
 * Expects a string like ".mat [rows][cols]"
 * Returns rows*cols if successful, 0 otherwise.
 */
int countMatValues(const char *line);
char *opCodes[num_of_opcodes] = { "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc","dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

char *skipWhitespace(char *line) {
    while (*line == ' ' || *line == '\t') {
        line++;
    }
    return line;
}

int isLabel(char *ptr) {
    int len = 0;
    while (ptr[len] && !isspace((unsigned char)ptr[len]) && ptr[len] != ':') {
        len++;
    }
    if (isalpha(ptr[0]) && ptr[len] == ':' && len > 0 && len <= MAX_LABEL_LENGTH) {
        return 1;
    }
    return 0;
}

int isData(char *word) {
    return (strcmp(word, ".data") == 0 ||
            strcmp(word, ".string") == 0 ||
            strcmp(word, ".mat") == 0);
}

int isInstruction(char *word) {
    int i;
    for (i = 0; i < num_of_opcodes; i++) {
        if (strcmp(word, opCodes[i]) == 0)
            return 1;
    }
    return 0;
}

int addSymbolToData(binTree **root, char *str, int address) {
    binTree *found;

    found = search(*root, str);
    if (found != NULL) {
        printf("Error symbol '%s' already exists!\n", str);
        return 0;
    }

    if (*root == NULL) {
        *root = makeNode(str, address, DATE, 0, 0);
        return 1;
    }

    addNode(*root, str, address, DATE, 0, 0);
    return 1;
}

int countDataValues(char *line) {
    int count = 0;
    const char *p = line;
    while (*p) {
        if (*p == ',' || *p == '-' || isdigit((unsigned char)*p)) {
            count++;

            while (*p && (isdigit((unsigned char)*p) || *p == '-' || *p == '+')) p++;

            while (*p && (isspace((unsigned char)*p) || *p == ',')) p++;
        } else {
            p++;
        }
    }
    return count;
}

int countWordsForInstruction(const char *line) {
    /*TODO:write this method*/
    return 1;
}

int countMatValues(const char *line) {
    int rows = 0, cols = 0;
    const char *p = strchr(line, '[');
    if (p) {
        rows = atoi(p + 1);
        p = strchr(p + 1, '[');
        if (p) {
            cols = atoi(p + 1);
        }
    }
    if (rows > 0 && cols > 0)
        return rows * cols;
    return 0;
}


int firstPass(const char *fileName, binTree **labelTable) {
    FILE *fp;
    int IC = IC_INIT_VALUE;
    int DC = DC_INIT_VALUE;
    char first_word[MAX_LABEL_LENGTH];
    char label[MAX_LABEL_LENGTH];
    int lineNumber = 1;
    int countError = 0;
    int tempMemory=0;
    LineDate currentLine;
    char *ptr;
    char *nextPtr;

    fp = fopen(fileName, "r");
    /*maybe switch to an ERROR method */
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

        if (isLabel(ptr)) {
            sscanf(ptr, "%[^:]:", label);
            currentLine.hasLabel = 1;
            strcpy(currentLine.label, label);

            nextPtr = strchr(ptr, ':');
            if (nextPtr != NULL) {
                nextPtr++;
                nextPtr = skipWhitespace(nextPtr);
                sscanf(nextPtr, "%s", first_word);
            } else {
                countError++;
                currentLine.hasError = 1;
                strcpy(currentLine.error, "Label not followed by command");
                lineNumber++;
                continue;
            }
        } else {
            sscanf(ptr, "%s", first_word);
        }

        if (strcmp(first_word, ".entry") == 0) {
            lineNumber++;
            continue;
        }
        if (strcmp(first_word, ".extern") == 0) {
            /*TODO:add to label table */
            lineNumber++;
            continue;
        }

        if (currentLine.hasLabel) {
            if (isData(first_word)) {
                addSymbolToData(labelTable, currentLine.label, DC);
                if (strcmp(first_word, ".data") == 0) {
                    tempMemory= countDataValues(nextPtr + strlen(first_word));
                    if (tempMemory == 0) {
                        strcpy(currentLine.error, "missing argument");
                        countError++;
                        return 0;
                    }
                    else {
                        DC+= tempMemory;
                    }
                } else if (strcmp(first_word, ".string") == 0) {
                    char *start = strchr(nextPtr, '\"');
                    if (start) {
                        char *end = strchr(start + 1, '\"');
                        if (end && end > start) {
                            DC += (end - start - 1) + 1;
                        }else {
                            strcpy(currentLine.error, "missing argument");
                            countError++;
                            return 0;
                        }
                    }
                } else if (strcmp(first_word, ".mat") == 0) {
                    tempMemory= countDataValues(currentLine.content);
                    if (tempMemory == 0) {
                        strcpy(currentLine.error, "missing argument");
                        countError++;
                        return 0;
                    }
                    else {
                        DC+= tempMemory;
                    }
                }
            } else if (isInstruction(first_word)) {
                addNode(*labelTable, currentLine.label, IC, CODE, 0, 0);
                IC += countWordsForInstruction(nextPtr + strlen(first_word));
            }
        } else {
            if (isData(first_word)) {
                if (strcmp(first_word, ".data") == 0) {
                    tempMemory= countDataValues(nextPtr + strlen(first_word));
                    if (tempMemory == 0) {
                        strcpy(currentLine.error, "missing argument");
                        countError++;
                        return 0;
                    }
                    else {
                        DC+= tempMemory;
                    }
                } else if (strcmp(first_word, ".string") == 0) {
                    char *start = strchr(nextPtr, '\"');
                    if (start) {
                        char *end = strchr(start + 1, '\"');
                        if (end && end > start) {
                            DC += (end - start - 1) + 1;
                        }
                    }
                } else if (strcmp(first_word, ".mat") == 0) {
                    tempMemory= countDataValues(currentLine.content);
                    if (tempMemory == 0) {
                        strcpy(currentLine.error, "missing argument");
                        countError++;
                        return 0;
                    }
                    else {
                        DC+= tempMemory;
                    }
                }
            } else if (isInstruction(first_word)) {
                IC += countWordsForInstruction(nextPtr + strlen(first_word));
            }
        }
        lineNumber++;
    }

    fclose(fp);

    if (countError > 0) {
        return countError;
    }
    return 0;
}

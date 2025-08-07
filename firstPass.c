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
 * Adds a data symbol to the symbol table
 * Returns 1 if it was added, 0 if not
 */
int addSymbolToData(binTree **root, char *str, int address);

/*
 * Returns the amount of numbers in the .data line
 */
int countDataValues(char *line);

/*
 * Counts the total memory required for a mat
 * Expects a string like ".mat [rows][cols]"
 * Returns rows*cols if successful, 0 otherwise.
 */
int countMatValues(const char *line);

int isMatrix(char *operand);
int isImmediate(char *operand);
int isRegister(char *operand);
/* Get register number */
int getRegisterNumber(char *operand);
/* Processes a single operand based on its addressing method */
void processSingleOperand(char *operand, int method, int *IC, lineNode **codeList, int lineNum);
/* Get opcode index from name */
int getOpcodeIndex(char *opcodeName);
/* Determine addressing method */
int getAddressingMethod(char *operand);

int addIC(binTree **root, int IC);
int addICList(lineNode *dataList, int IC_FINAL);
int hasOnlyDestOperand(char *opcodeName);
char *opCodes[num_of_opcodes] = { "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

char *skipWhitespace(char *line) {
    while (*line == ' ' || *line == '\t') line++;
    return line;
}

int isLabel(char *ptr) {
    int len = 0;
    while (ptr[len] && !isspace((unsigned char)ptr[len]) && ptr[len] != ':') len++;
    return (ptr[len] == ':' && check_labelName(ptr) != ERROR);

    if (ptr[len] == ':' && check_labelName(ptr) == ERROR)
        return ERROR;

    return ptr[len] == ':';
}

int isData(char *word) {
    return (strcmp(word, ".data") == 0 || strcmp(word, ".string") == 0 || strcmp(word, ".mat") == 0);
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
    binTree *found = search(*root, str);
    if (found != NULL) {
        printf("Error symbol '%s' already exists!\n", str);
        return 0;
    }
    if (*root == NULL)
        *root = makeNode(str, address, DATE, 0, 0);
    else
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
        } else p++;
    }
    return count;
}

int countMatValues(const char *line) {
    int rows = 0, cols = 0;
    const char *p = strchr(line, '[');
    if (p) {
        rows = atoi(p + 1);
        p = strchr(p + 1, '[');
        if (p) cols = atoi(p + 1);
    }
    return (rows > 0 && cols > 0) ? rows * cols : 0;
}

int isRegister(char *operand) {
    return (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0');
}

int isImmediate(char *operand) {
    return (operand[0] == '#');
}

int isMatrix(char *operand) {
    return (strchr(operand, '[') != NULL && strchr(operand, ']') != NULL);
}

void intToBinary(int value, char *binaryLine) {
    int i;
    unsigned int temp = 1 << (WORD_LENGTH - 1);
    for (i = 0; i < WORD_LENGTH; i++) {
        binaryLine[i] = (value & temp) ? '1' : '0';
        temp >>= 1;
    }
    binaryLine[WORD_LENGTH] = '\0';
}

void processStringDirective(char *data, int *DC, lineNode **dataList, int lineNum) {
    char *start = strchr(data, '"');
    char *end = strchr(start + 1, '"');
    char binaryLine[WORD_LENGTH + 1];
    int i;
    if (!start || !end || end <= start) {
        printf("Error: Invalid string format at line %d\n", lineNum);
        return;
    }
    start++;
    for (i = 0; start[i] && &start[i] < end; i++) {
        intToBinary((int)start[i], binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
    }
    intToBinary(0, binaryLine);
    addLineNode(dataList, binaryLine, (*DC)++, lineNum);
}

void processDataDirective(char *data, int *DC, lineNode **dataList, int lineNum) {
    char *token = strtok(data, ", \t\n");
    char binaryLine[WORD_LENGTH + 1];
    while (token) {
        intToBinary(atoi(token), binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
        token = strtok(NULL, ", \t\n");
    }
}

void processMatDirective(char *line, int *DC, lineNode **dataList, int lineNum) {
    char *p = strchr(line, '[');
    int rows = 0, cols = 0, total, count = 0;
    char binaryLine[WORD_LENGTH + 1];
    if (p) {
        rows = atoi(p + 1);
        p = strchr(p + 1, '[');
        if (p) cols = atoi(p + 1);
    }
    if (rows <= 0 || cols <= 0) {
        printf("Error: Invalid matrix dimensions at line %d\n", lineNum);
        return;
    }
    total = rows * cols;
    p = strchr(p, ']');
    if (p) p = strchr(p + 1, ']');
    if (p) p++;
    char *token = strtok(p, ", \t\n");
    while (token && count < total) {
        intToBinary(atoi(token), binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
        token = strtok(NULL, ", \t\n");
        count++;
    }
    while (count < total) {
        intToBinary(0, binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
        count++;
    }
}

void processDataLine(char *line, int *DC, lineNode **dataList, int lineNum) {
    char directive[MAX_LINE_LENGTH];
    char *restOfLine;
    sscanf(line, "%s", directive);
    restOfLine = strchr(line, ' ');
    if (restOfLine) restOfLine = skipWhitespace(restOfLine);
    if (strcmp(directive, ".string") == 0)
        processStringDirective(restOfLine, DC, dataList, lineNum);
    else if (strcmp(directive, ".data") == 0)
        processDataDirective(restOfLine, DC, dataList, lineNum);
    else if (strcmp(directive, ".mat") == 0)
        processMatDirective(restOfLine, DC, dataList, lineNum);
}


/* Encodes the first instruction word */
void encodeInstructionWord(int opcode, int srcMethod, int destMethod, char *binaryLine) {
    int instruction = 0;
    instruction |= (opcode & 0xF) << 8;
    instruction |= (srcMethod & 0x3) << 6;
    instruction |= (destMethod & 0x3) << 4;
    instruction |= 0;
    intToBinary(instruction, binaryLine);
}

/* Encodes a single operand word */
void encodeOperandWord(int value, int addressingMethod, char *binaryLine) {
    int operand = 0;
    operand |= (value & 0x3FF) << 2;
    operand |= (addressingMethod == 3) ? 2 : 0;
    intToBinary(operand, binaryLine);
}

/* Processes an instruction line */
void processInstructionLine(char *opcode, operands ops, int *IC, lineNode **codeList, int lineNum) {
    int opcodeIndex = getOpcodeIndex(opcode);
    int srcMethod = (ops.operandCount == 2) ? getAddressingMethod(ops.op1) : 0;
    int destMethod = (ops.operandCount >= 1) ? getAddressingMethod(ops.op2) : 0;
    char binaryLine[WORD_LENGTH + 1];

    encodeInstructionWord(opcodeIndex, srcMethod, destMethod, binaryLine);
    addLineNode(codeList, binaryLine, (*IC)++, lineNum);

    if (ops.operandCount == 2) {
        processSingleOperand(ops.op1, srcMethod, IC, codeList, lineNum);
        processSingleOperand(ops.op2, destMethod, IC, codeList, lineNum);
    } else if (ops.operandCount == 1) {
        processSingleOperand(ops.op2, destMethod, IC, codeList, lineNum);
    }
}

/* Processes a single operand based on its addressing method */
void processSingleOperand(char *operand, int method, int *IC, lineNode **codeList, int lineNum) {
    char binaryLine[WORD_LENGTH + 1];
    int value = 0;

    if (method == 0) {
        value = atoi(operand + 1);
        encodeOperandWord(value, method, binaryLine);
    } else if (method == 1 || method == 2) {
        encodeOperandWord(0, method, binaryLine);
    } else if (method == 3) {
        value = getRegisterNumber(operand);
        encodeOperandWord(value, method, binaryLine);
    }
    addLineNode(codeList, binaryLine, (*IC)++, lineNum);
}

/* Get opcode index from name */
int getOpcodeIndex(char *opcodeName) {
    int i;
    for (i = 0; i < 16; i++) {
        if (strcmp(opcodeTable[i].name, opcodeName) == 0)
            return i;
    }
    return -1;
}

/* Determine addressing method */
int getAddressingMethod(char *operand) {
    if (operand[0] == '#') return 0;
    if (strchr(operand, '[') != NULL) return 2;
    if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7') return 3;
    return 1;
}

/* Get register number */
int getRegisterNumber(char *operand) {
    return operand[1] - '0';
}

operands parseOperands(char *lineAfterOpcode, char *opcodeName) {
    operands ops;
    char tempLine[MAX_LINE_LENGTH];
    char *token;
    int count = 0;

    ops.op1[0] = '\0';
    ops.op2[0] = '\0';
    ops.operandCount = 0;

    strcpy(tempLine, lineAfterOpcode);

    token = strtok(tempLine, " ,\t\n");
    if (token != NULL) {
        if (hasOnlyDestOperand(opcodeName)) {
            strcpy(ops.op2, token);
        } else {
            strcpy(ops.op1, token);
        }
        count++;
    }

    token = strtok(NULL, " ,\t\n");
    if (token != NULL) {
        strcpy(ops.op2, token);
        count++;
    }

    ops.operandCount = count;
    return ops;
}

int hasOnlyDestOperand(char *opcodeName) {
    return (strcmp(opcodeName, "clr") == 0 || strcmp(opcodeName, "not") == 0 || strcmp(opcodeName, "inc") == 0 || strcmp(opcodeName, "dec") == 0 ||
        strcmp(opcodeName, "jmp") == 0 || strcmp(opcodeName, "bne") == 0 || strcmp(opcodeName, "jsr") == 0 || strcmp(opcodeName, "red") == 0 ||
        strcmp(opcodeName, "prn") == 0);
}


/*gets 2 lists and chains them together */
lineNode *concatLists(lineNode *list1, lineNode *list2) {
    lineNode *p;
    if (list1 == NULL) {
        return list2;
    }
    p = list1;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = list2;
    return list1;
}
/* -------------------------------------------------------------------------------------------------*/
/* in main need to reset the labelTable and the two lists */
int firstPass(int index) {
    FILE *fp;
    int IC = IC_INIT_VALUE;
    int DC = DC_INIT_VALUE;
    char first_word[MAX_LABEL_LENGTH];
    char label[MAX_LABEL_LENGTH];
    int lineNumber = 1;
    int countError = 0;
    LineDate currentLine;
    char *ptr;
    char *nextPtr;
    int readLine;
    const char *fileName = nameArr[index];
    operands operands;
    lineNode *codeList = NULL;
    lineNode *dataList = NULL;
    binTree **curLabelTable = &labelTable[index];



    fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("Error opening file %s\n", fileName);
        return -1;
    }

    readLine = takeInLine(currentLine.content, fp);
    while (readLine != EOF) {
        if (readLine != 0) {
            countError++;
            lineNumber++;
            readLine = takeInLine(currentLine.content, fp);
            continue;
        }

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
                readLine = takeInLine(currentLine.content, fp);
                continue;
            }
        } else {
            sscanf(ptr, "%s", first_word);
            nextPtr = skipWhitespace(ptr + strlen(first_word));
        }

        if (strcmp(first_word, ".entry") == 0) {
            nextPtr = skipWhitespace(ptr + strlen(".entry"));
            sscanf(nextPtr, "%s", first_word);
            addLineNode(&entryLineArr[index], first_word, 0, lineNumber);
            lineNumber++;
            readLine = takeInLine(currentLine.content, fp);
            continue;
        }

        if (strcmp(first_word, ".extern") == 0) {
            sscanf(nextPtr, "%s", currentLine.label);
            addNode(curLabelTable, currentLine.label, 0, 3, 1, 0);
            lineNumber++;
            readLine = takeInLine(currentLine.content, fp);
            continue;
        }

        if (isData(first_word)) {
            if (currentLine.hasLabel) {
                addSymbolToData(curLabelTable, currentLine.label, DC);
            }
            processDataLine(nextPtr, &DC, &dataList, lineNumber);
        } else if (isInstruction(first_word)) {
            if (currentLine.hasLabel) {
                addNode(*curLabelTable, currentLine.label, IC, CODE, 0, 0);
            }
            operands = parseOperands(nextPtr, first_word);
            processInstructionLine(first_word, operands, &IC, &codeList, lineNumber);
        } else {
            strcpy(currentLine.error, "Unknown command");
            currentLine.hasError = 1;
            countError++;
        }

        lineNumber++;
        readLine = takeInLine(currentLine.content, fp);
    }
    addIC(curLabelTable, IC);
    addICList(dataList, IC);

    lineArr[index] = concatLists(codeList, dataList);

    fclose(fp);

    dcArr[index]=DC;
    icArr[index]=IC;

    if (IC+DC<MAX_TOTAL_ADDRESSES) {
        printf("Program size (IC + DC = %d) exceeds maximum memory limit (%d) \n", (IC + DC), MAX_TOTAL_ADDRESSES);
        countError++;
    }
    
    if (countError > 0) {
        return countError;
    }
    return 0;
}

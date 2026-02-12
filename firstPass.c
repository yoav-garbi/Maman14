#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "prototypes.h"

/* quick check for data directives */
int isData(char *word);
int isInstruction(char *word);
char *skipWhitespace(char *line);
int isLabel(char *ptr);
int addSymbolToData(binTree **root, char *str, int address);
int countDataValues(char *line);
int countMatValues(const char *line);
int isMatrix(char *operand);
int isImmediate(char *operand);
int isRegister(char *operand);
int getRegisterNumber(char *operand);
/* Processes a single operand based on its addressing method */
void processSingleOperand(char *operand, int method, int *IC, lineNode **codeList, int lineNum, binTree *labelTable, lineNode **externLineArr);
/* Get opcode index from name */
int getOpcodeIndex(char *opcodeName);
/* Determine addressing method */
int getAddressingMethod(char *operand);
int addIC(binTree **root, int IC);
int addICList(lineNode *dataList, int IC_FINAL);
int hasOnlyDestOperand(char *opcodeName);
void addExternIfNeeded(char *operand, int IC, binTree *labelTable, lineNode **externLineArr);
void processInstructionLine(char *opcode, operands ops, int *IC, lineNode **codeList, int lineNum, binTree *labelTable, lineNode **externLineArr);

char *opCodes[num_of_opcodes] = { "mov", "cmp", "add", "sub", "not", "clr",
    "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

char *skipWhitespace(char *line) {
    while (*line == ' ' || *line == '\t')
        line++;
    return line;
}

int isLabel(char *ptr) {
    int len = 0;

    /* find end of potential label */
    while (ptr[len] && !isspace((unsigned char)ptr[len]) && ptr[len] != ':')
        len++;

    if (ptr[len] != ':')
        return 0;

    /* TODO: check label validity */
    if (check_labelName(ptr) == ERROR) {
        return ERROR;
    }

    return 1;
}

int isData(char *word) {
    if (strcmp(word, ".data") == 0) return 1;
    if (strcmp(word, ".string") == 0) return 1;
    if (strcmp(word, ".mat") == 0) return 1;
    return 0;
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
        return 0; /* already there */
    }

    if (*root == NULL)
        *root = makeNode(str, address, DATE, 0, 0);
    else
        addNode(*root, str, address, DATE, 0, 0);
    return 1;
}

/* count numbers in data line - bit hacky but works */
int countDataValues(char *line) {
    int count = 0;
    const char *p = line;

    while (*p) {
        /* skip whitespace */
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;

        /* found a number? */
        if (*p == '-' || *p == '+' || isdigit((unsigned char)*p)) {
            count++;
            /* skip the number */
            if (*p == '-' || *p == '+') p++;
            while (*p && isdigit((unsigned char)*p)) p++;
        } else if (*p == ',') {
            p++; /* just skip comma */
        } else {
            p++; /* skip */
        }
    }
    return count;
}

int countMatValues(const char *line) {
    int rows = 0, cols = 0;
    const char *p;

    p = strchr(line, '[');
    if (!p) return 0;

    rows = atoi(p + 1);
    p = strchr(p + 1, '[');
    if (!p) return 0;

    cols = atoi(p + 1);
    return (rows > 0 && cols > 0) ? rows * cols : 0;
}

int isRegister(char *operand) {
    return (operand[0] == 'r' && operand[1] >= '0' &&
            operand[1] <= '7' && operand[2] == '\0');
}

int isImmediate(char *operand) {
    return (operand[0] == '#');
}

int isMatrix(char *operand) {
    return (strchr(operand, '[') != NULL && strchr(operand, ']') != NULL);
}

/* convert int to binary string */
void intToBinary(int value, char *binaryLine) {
    int i;
    unsigned int mask = 1 << (WORD_LENGTH - 1);

    for (i = 0; i < WORD_LENGTH; i++) {
        binaryLine[i] = (value & mask) ? '1' : '0';
        mask >>= 1;
    }
    binaryLine[WORD_LENGTH] = '\0';
}

void processStringDirective(char *data, int *DC, lineNode **dataList, int lineNum) {
    char *start, *end;
    char binaryLine[WORD_LENGTH + 1];
    int i;

    start = strchr(data, '"');
    if (!start) {
        printf("Error: No opening quote at line %d\n", lineNum);
        return;
    }

    end = strchr(start + 1, '"');
    if (!end || end <= start) {
        printf("Error: Invalid string format at line %d\n", lineNum);
        return;
    }

    start++; /* skip opening quote */

    /* add each character */
    for (i = 0; start + i < end; i++) {
        intToBinary((int)start[i], binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
    }

    /* null terminator */
    intToBinary(0, binaryLine);
    addLineNode(dataList, binaryLine, (*DC)++, lineNum);
}

void processDataDirective(char *data, int *DC, lineNode **dataList, int lineNum) {
    char *token;
    char binaryLine[WORD_LENGTH + 1];

    token = strtok(data, ", \t\n");
    while (token) {
        intToBinary(atoi(token), binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
        token = strtok(NULL, ", \t\n");
    }
}

void processMatDirective(char *line, int *DC, lineNode **dataList, int lineNum) {
    char *p;
    int rows = 0, cols = 0, total, count = 0;
    char binaryLine[WORD_LENGTH + 1];
    char *token;

    /* parse dimensions */
    p = strchr(line, '[');
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

    /* find start of data */
    p = strchr(p, ']');
    if (p) p = strchr(p + 1, ']');
    if (p) p++;

    /* process values */
    token = strtok(p, ", \t\n");
    while (token && count < total) {
        intToBinary(atoi(token), binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
        token = strtok(NULL, ", \t\n");
        count++;
    }

    /* pad with zeros if needed */
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
    if (restOfLine)
        restOfLine = skipWhitespace(restOfLine);

    if (strcmp(directive, ".string") == 0) {
        processStringDirective(restOfLine, DC, dataList, lineNum);
    } else if (strcmp(directive, ".data") == 0) {
        processDataDirective(restOfLine, DC, dataList, lineNum);
    } else if (strcmp(directive, ".mat") == 0) {
        processMatDirective(restOfLine, DC, dataList, lineNum);
    }
}

/* encode instruction word */
void encodeInstructionWord(int opcode, int srcMethod, int destMethod, char *binaryLine) {
    int instruction = 0;

    instruction |= (opcode & 0xF) << 8;
    instruction |= (srcMethod & 0x3) << 6;
    instruction |= (destMethod & 0x3) << 4;
    /* ERA bits are 0 for now */

    intToBinary(instruction, binaryLine);
}

/* encode operand word */
void encodeOperandWord(int value, int addressingMethod, char *binaryLine) {
    int operand = 0;

    operand |= (value & 0x3FF) << 2; /* 10 bits for value */
    operand |= (addressingMethod == 3) ? 2 : 0; /* ERA bits */

    intToBinary(operand, binaryLine);
}

/* Adds an extern operand to externLineArr if needed */
void addExternIfNeeded(char *operand, int IC, binTree *labelTable, lineNode **externLineArr) {
    if (operand == NULL) return;

    binTree *sym = search(labelTable, operand);
    if (sym != NULL && sym->symbolType == EXTERN) {
        addLineNode(externLineArr, operand, IC, 0);
    }
}

/* Processes a single operand based on its addressing method */
void processSingleOperand(char *operand, int method, int *IC, lineNode **codeList, int lineNum,
                          binTree *labelTable, lineNode **externLineArr) {
    char binaryLine[WORD_LENGTH + 1];
    char labelPadded[MAX_LABEL_LENGTH + 2];
    int value = 0;

    switch (method) {
        case 0: /* immediate */
            value = atoi(operand + 1); /* skip the # */
            encodeOperandWord(value, method, binaryLine);
            addLineNode(codeList, binaryLine, (*IC)++, lineNum);
            break;

        case 1: /* direct */
        case 2: /* matrix */
            addExternIfNeeded(operand, *IC, labelTable, externLineArr);
            sprintf(labelPadded, " %-8s ", operand);
            addLineNode(codeList, labelPadded, (*IC)++, lineNum);
            break;

        case 3: /* register */
            value = getRegisterNumber(operand);
            encodeOperandWord(value, method, binaryLine);
            addLineNode(codeList, binaryLine, (*IC)++, lineNum);
            break;
    }
}

/* Processes an instruction line */
void processInstructionLine(char *opcode, operands ops, int *IC, lineNode **codeList, int lineNum,
                            binTree *labelTable, lineNode **externLineArr) {
    int opcodeIndex = getOpcodeIndex(opcode);
    int srcMethod = 0, destMethod = 0;
    char binaryLine[WORD_LENGTH + 1];

    if (ops.operandCount == 2) {
        srcMethod = getAddressingMethod(ops.op1);
        destMethod = getAddressingMethod(ops.op2);
    } else if (ops.operandCount == 1) {
        destMethod = getAddressingMethod(ops.op2);
    }

    encodeInstructionWord(opcodeIndex, srcMethod, destMethod, binaryLine);
    addLineNode(codeList, binaryLine, (*IC)++, lineNum);

    if (ops.operandCount == 2) {
        processSingleOperand(ops.op1, srcMethod, IC, codeList, lineNum, labelTable, externLineArr);
        processSingleOperand(ops.op2, destMethod, IC, codeList, lineNum, labelTable, externLineArr);
    } else if (ops.operandCount == 1) {
        processSingleOperand(ops.op2, destMethod, IC, codeList, lineNum, labelTable, externLineArr);
    }
}

int getOpcodeIndex(char *opcodeName) {
    int i;
    for (i = 0; i < 16; i++) {
        if (strcmp(opcodeTable[i].name, opcodeName) == 0)
            return i;
    }
    return -1;
}

int getAddressingMethod(char *operand) {
    if (operand[0] == '#') return 0;
    if (strchr(operand, '[') != NULL) return 2;
    if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7') return 3;
    return 1;
}

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
    if (strcmp(opcodeName, "clr") == 0) return 1;
    if (strcmp(opcodeName, "not") == 0) return 1;
    if (strcmp(opcodeName, "inc") == 0) return 1;
    if (strcmp(opcodeName, "dec") == 0) return 1;
    if (strcmp(opcodeName, "jmp") == 0) return 1;
    if (strcmp(opcodeName, "bne") == 0) return 1;
    if (strcmp(opcodeName, "jsr") == 0) return 1;
    if (strcmp(opcodeName, "red") == 0) return 1;
    if (strcmp(opcodeName, "prn") == 0) return 1;
    return 0;
}

lineNode *concatLists(lineNode *list1, lineNode *list2) {
    lineNode *p;

    if (list1 == NULL) return list2;

    p = list1;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = list2;
    return list1;
}

int firstPass(int index) {
    FILE *fp;
    int IC = IC_INIT_VALUE;
    int DC = DC_INIT_VALUE;
    char first_word[MAX_LABEL_LENGTH];
    char label[MAX_LABEL_LENGTH];
    int lineNumber = 1;
    int countError = 0;
    LineData currentLine;
    char *ptr, *nextPtr;
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
            addNode(curLabelTable, currentLine.label, 0, EXTERN, 1, 0);
            lineNumber++;
            readLine = takeInLine(currentLine.content, fp);
            continue;
        }

        if (isData(first_word)) {
            if (currentLine.hasLabel) {
                addSymbolToData(curLabelTable, currentLine.label, DC);
            }
            processDataLine(nextPtr, &DC, &dataList, lineNumber);
        }
        else if (isInstruction(first_word)) {
            if (currentLine.hasLabel) {
                addNode(*curLabelTable, currentLine.label, IC, CODE, 0, 0);
            }
            operands = parseOperands(nextPtr, first_word);
            processInstructionLine(first_word, operands, &IC, &codeList, lineNumber, *curLabelTable, &externLineArr[index]);
        }
        else {
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

    dcArr[index] = DC;
    icArr[index] = IC;

    if (IC + DC >= MAX_TOTAL_ADDRESSES) {
        printf("Program size (IC + DC = %d) exceeds maximum memory limit (%d)\n",
               (IC + DC), MAX_TOTAL_ADDRESSES);
        countError++;
    }

    return countError;
}

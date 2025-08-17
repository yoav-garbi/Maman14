#include "prototypes.h"

/*
 * =====================================================================
 *  First Pass (Assembler) — Responsibilities
 * =====================================================================
 *  - Read the .am file line-by-line
 *  - Run surface-level syntax checks (errors.c) so we get precise,
 *    human-friendly error messages with correct line numbers
 *  - Build the symbol table (labels for CODE/DATA, EXTERN placeholders),
 *    record .entry/.extern mentions, and note external usages for .ext
 *  - Schemati­cally encode instruction/data words to count how many words
 *    each source line contributes (IC/DC growth)
 *  - Track IC (code words) and DC (data words); at end, offset DATA by IC
 *  - Fail gracefully if total (IC + DC) exceeds memory limits
 *
 *  Conventions / Constraints:
 *  - C90 compliant: only block comments, declarations before statements
 *  - We keep the global 'lineCounter' in sync so errors from errors.c
 *    point to the exact line we’re on
 */

/* Supported opcode names (mirrors opcodeTable for convenience in predicates) */
char *opCodes[num_of_opcodes] = {
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc",
    "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
};

/* Skip spaces and tabs; returns first non-space character */
char *skipWhitespace(char *line)
{
    while (*line == ' ' || *line == '\t')
        line++;
    return line;
}

/* Check if token at 'ptr' is a label (ends with ':') and that its name is legal
 * Returns: 1 if valid label, 0 if not a label, ERROR if invalid label name */
int isLabel(char *ptr)
{
    int len = 0;
    char *colon;
    char saved;

    while (ptr[len] && !isspace((unsigned char)ptr[len]) && ptr[len] != ':')
        len++;

    if (ptr[len] != ':')
        return 0;

    /* Temporarily cut right after ':' so check_labelName sees "name:" */
    colon = ptr + len;
    saved = colon[1];
    colon[1] = '\0';

    if (check_labelName(ptr) == ERROR) {
        colon[1] = saved;
        return ERROR;
    }

    colon[1] = saved;
    return 1;
}

/* Is this a data directive (.data/.string/.mat)? */
int isData(char *word)
{
    if (strcmp(word, ".data") == 0)   return 1;
    if (strcmp(word, ".string") == 0) return 1;
    if (strcmp(word, ".mat") == 0)    return 1;
    return 0;
}

/* Is this a legal instruction mnemonic? */
int isInstruction(char *word)
{
    int i;
    for (i = 0; i < num_of_opcodes; i++) {
        if (strcmp(word, opCodes[i]) == 0)
            return 1;
    }
    return 0;
}

/* Add a DATA symbol to the symbol table if it does not exist yet */
int addSymbolToData(binTree **root, char *str, int address)
{
    binTree *found = search(*root, str);
    if (found != NULL)
        return 0;

    if (*root == NULL)
        *root = makeNode(str, address, DATA, 0, 0);
    else
        addNode(root, str, address, DATA, 0, 0);
    return 1;
}

/* Convert integer to fixed-width binary (WORD_LENGTH) string */
void intToBinary(int value, char *binaryLine)
{
    int i;
    unsigned int mask = 1U << (WORD_LENGTH - 1);
    for (i = 0; i < WORD_LENGTH; i++) {
        binaryLine[i] = (value & mask) ? '1' : '0';
        mask >>= 1U;
    }
    binaryLine[WORD_LENGTH] = '\0';
}

/* Data directives -----------------------------------------------------------------------------------------------*/

/* .string — write each char as a word, then a terminating 0 word */
void processStringDirective(char *data, int *DC, lineNode **dataList, int lineNum)
{
    char *start, *end;
    char binaryLine[WORD_LENGTH + 1];
    int i;

    start = strchr(data, '"');
    if (!start) return;

    end = strchr(start + 1, '"');
    if (!end || end <= start) return;

    start++; /* skip opening quote */

    for (i = 0; start + i < end; i++) {
        intToBinary((int)start[i], binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
    }

    /* NUL terminator */
    intToBinary(0, binaryLine);
    addLineNode(dataList, binaryLine, (*DC)++, lineNum);
}

/* .data — parse comma-separated integers, each becomes one word */
void processDataDirective(char *data, int *DC, lineNode **dataList, int lineNum)
{
    char *token;
    char binaryLine[WORD_LENGTH + 1];

    token = strtok(data, ", \t\n");
    while (token) {
        intToBinary(atoi(token), binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
        token = strtok(NULL, ", \t\n");
    }
}

/* .mat — parse dimensions [rows][cols], then values
 * If too few are provided,
 * pad with zeros up to rows*cols */
void processMatDirective(char *line, int *DC, lineNode **dataList, int lineNum)
{
    const char *p;
    const char *q;
    int rows, cols, capacity, written;
    int sign, val, have_digit;
    char binaryLine[WORD_LENGTH + 1];

    p = line;
    rows = 0;
    cols = 0;
    capacity = 0;
    written = 0;
    sign = 1;
    val = 0;
    have_digit = 0;

    /* Dimensions: .mat [rows][cols] */
    p = strchr(p, '[');
    if (p == NULL) return;
    rows = atoi(p + 1);

    p = strchr(p + 1, '[');
    if (p == NULL) return;
    cols = atoi(p + 1);

    if (rows <= 0 || cols <= 0) return;
    capacity = rows * cols;

    /* Move beyond the last ']' of the dimensions */
    p = strchr(p, ']');
    if (p == NULL) return;
    p++;

    /* Skip whitespace before values */
    while (*p == ' ' || *p == '\t') p++;

    /* Read up to capacity values */
    while (*p != '\0' && written < capacity) {
        while (*p == ' ' || *p == '\t' || *p == ',')
            p++;

        if (*p == '\0' || *p == '\n')
            break;

        q = p;
        sign = 1;
        val = 0;
        have_digit = 0;

        if (*q == '+' || *q == '-') {
            if (*q == '-') sign = -1;
            q++;
        }

        while (isdigit((unsigned char)*q)) {
            have_digit = 1;
            val = val * 10 + (int)(*q - '0');
            q++;
        }

        if (!have_digit)
            break;

        val *= sign;

        intToBinary(val, binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
        written++;

        p = q;
    }

    /* Zero-fill if fewer values were specified than rows*cols */
    while (written < capacity) {
        intToBinary(0, binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
        written++;
    }
}

/* Dispatch a data directive to the correct handler
 * Also tolerant to either space or tab after the directive keyword */
void processDataLine(char *line, int *DC, lineNode **dataList, int lineNum)
{
    char directive[MAX_LINE_LENGTH];
    char *restOfLine;

    sscanf(line, "%s", directive);

    restOfLine = line;
    while (*restOfLine && !isspace((unsigned char)*restOfLine))
        restOfLine++;
    if (*restOfLine)
        restOfLine = skipWhitespace(restOfLine);

    if (strcmp(directive, ".string") == 0)
        processStringDirective(restOfLine, DC, dataList, lineNum);
    else if (strcmp(directive, ".data") == 0)
        processDataDirective(restOfLine, DC, dataList, lineNum);
    else if (strcmp(directive, ".mat") == 0)
        processMatDirective(restOfLine, DC, dataList, lineNum);
}

 /* Instruction/operand encoding (schematic for first pass) --------------------------------------------------------------------------*/

/* Instruction word layout:
 *   bits 6-9: opcode (4)
 *   bits 4-5: src addressing (2)
 *   bits 2-3: dst addressing (2)
 *   bits 0-1: ERA (0 for now)
 */
void encodeInstructionWord(int opcode, int srcMethod, int destMethod, char *binaryLine)
{
    int instruction = 0;
    instruction |= (opcode     & 0xF) << 6;
    instruction |= (srcMethod  & 0x3) << 4;
    instruction |= (destMethod & 0x3) << 2;
    intToBinary(instruction, binaryLine);
}

/* Operand word layout (simplified for pass one):
 *   bits 2-9: value (8)
 *   bits 0-1: ERA (set to 2 for register addressing here as a placeholder)
 */
void encodeOperandWord(int value, int addressingMethod, char *binaryLine)
{
    int operand = 0;
    operand |= (value & 0xFF) << 2;
    operand |= (addressingMethod == 3) ? 2 : 0;
    intToBinary(operand, binaryLine);
}

/* If operand names an external symbol, record its usage for .ext output */
void addExternIfNeeded(char *operand, int IC, binTree *oneLabelTable, lineNode **externList)
{
    binTree *sym;
    if (operand == NULL) return;

    sym = search(oneLabelTable, operand);
    if (sym != NULL && sym->isExternal) {
        addLineNode(externList, operand, IC, 0);
    }
}

/* Encode a single operand according to its addressing method */
void processSingleOperand(char *operand, int method, int *IC, lineNode **codeList,
                          int lineNum, binTree *oneLabelTable, lineNode **externList)
{
    char binaryLine[WORD_LENGTH + 1];
    char labelPadded[MAX_LABEL_LENGTH + 2];
    char matLabel[MAX_LABEL_LENGTH + 1];
    char rowReg[3], colReg[3];
    int value = 0;
    int width;

    switch (method) {
        case 0: /* #immediate */
            value = atoi(operand + 1);
            encodeOperandWord(value, method, binaryLine);
            addLineNode(codeList, binaryLine, (*IC)++, lineNum);
            break;

        case 1: /* direct label */
            addExternIfNeeded(operand, *IC, oneLabelTable, externList);
            /* Guard the formatted width so we never overflow the buffer */
            width = address_binary_representation_size;
            if (width > (MAX_LABEL_LENGTH + 1))
                width = (MAX_LABEL_LENGTH + 1);
            sprintf(labelPadded, " %-*.*s ", width, width, operand);
            addLineNode(codeList, labelPadded, (*IC)++, lineNum);
            break;

        case 2: /* matrix: LABEL[rX][rY] => LABEL + two register words */
            if (sscanf(operand, "%[^[][%2[^]]][%2[^]]]", matLabel, rowReg, colReg) == 3) {
                addExternIfNeeded(matLabel, *IC, oneLabelTable, externList);

                width = address_binary_representation_size;
                if (width > (MAX_LABEL_LENGTH + 1))
                    width = (MAX_LABEL_LENGTH + 1);
                sprintf(labelPadded, " %-*.*s ", width, width, matLabel);
                addLineNode(codeList, labelPadded, (*IC)++, lineNum);

                value = getRegisterNumber(rowReg);
                encodeOperandWord(value, 3, binaryLine);
                addLineNode(codeList, binaryLine, (*IC)++, lineNum);

                value = getRegisterNumber(colReg);
                encodeOperandWord(value, 3, binaryLine);
                addLineNode(codeList, binaryLine, (*IC)++, lineNum);
            }
            break;

        case 3: /* register: r0..r7 */
            value = getRegisterNumber(operand);
            encodeOperandWord(value, method, binaryLine);
            addLineNode(codeList, binaryLine, (*IC)++, lineNum);
            break;
    }
}

/* Put together one instruction line: instruction word + its operands */
void processInstructionLine(char *opcode, operands ops, int *IC, lineNode **codeList,
                            int lineNum, binTree *oneLabelTable, lineNode **externList)
{
    int opcodeIndex = getOpcodeIndex(opcode);
    int srcMethod = 0, destMethod = 0;
    char binaryLine[WORD_LENGTH + 1];

    if (ops.operandCount == 2) {
        srcMethod  = getAddressingMethod(ops.op1);
        destMethod = getAddressingMethod(ops.op2);
    } else if (ops.operandCount == 1) {
        destMethod = getAddressingMethod(ops.op2);
    }

    encodeInstructionWord(opcodeIndex, srcMethod, destMethod, binaryLine);
    addLineNode(codeList, binaryLine, (*IC)++, lineNum);

    if (ops.operandCount == 2) {
        processSingleOperand(ops.op1, srcMethod,  IC, codeList, lineNum, oneLabelTable, externList);
        processSingleOperand(ops.op2, destMethod, IC, codeList, lineNum, oneLabelTable, externList);
    } else if (ops.operandCount == 1) {
        processSingleOperand(ops.op2, destMethod, IC, codeList, lineNum, oneLabelTable, externList);
    }
}

 /* Opcode/operand utilities -------------------------------------------------------------------------------*/

/* Find opcode index by name (or -1 if not found) */
int getOpcodeIndex(char *opcodeName)
{
    int i;
    for (i = 0; i < 16; i++) {
        if (strcmp(opcodeTable[i].name, opcodeName) == 0)
            return i;
    }
    return -1;
}

/* Addressing method classifier:
 *   0 = immediate (#n), 1 = direct (label), 2 = matrix (label[r][r]), 3 = register (r0..r7)
 */
int getAddressingMethod(char *operand)
{
    if (operand[0] == '#') return 0;
    if (strchr(operand, '[') != NULL) return 2;
    if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7') return 3;
    return 1;
}

/* Extract register number from "rX" */
int getRegisterNumber(char *operand)
{
    return operand[1] - '0';
}

/* Parse operands after the opcode. For single-operand instructions */
operands parseOperands(char *lineAfterOpcode, char *opcodeName)
{
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
            strncpy(ops.op2, token, MAX_LABEL_LENGTH - 1);
            ops.op2[MAX_LABEL_LENGTH - 1] = '\0';
        } else {
            strncpy(ops.op1, token, MAX_LABEL_LENGTH - 1);
            ops.op1[MAX_LABEL_LENGTH - 1] = '\0';
        }
        count++;
    }

    token = strtok(NULL, " ,\t\n");
    if (token != NULL) {
        strncpy(ops.op2, token, MAX_LABEL_LENGTH - 1);
        ops.op2[MAX_LABEL_LENGTH - 1] = '\0';
        count++;
    }

    ops.operandCount = count;
    return ops;
}

/* Return 1 if the opcode takes only a destination operand */
int hasOnlyDestOperand(char *opcodeName)
{
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

/* Concatenate two linked lists (append list2 to list1) */
lineNode *concatLists(lineNode *list1, lineNode *list2)
{
    lineNode *p;

    if (list1 == NULL) return list2;

    p = list1;
    while (p->next != NULL)
        p = p->next;
    p->next = list2;
    return list1;
}

 /* --------------------------------------------------------------------------------------------*/
int firstPass(int index)
{
    FILE *fp;
    int IC = IC_INIT_VALUE;
    int DC = DC_INIT_VALUE;
    char first_word[MAX_LABEL_LENGTH];
    char label[MAX_LABEL_LENGTH];
    int lineNumber = 1;
    int countError = 0;
    LineData currentLine;
    char *ptr;
    char *nextPtr;
    int readLine;
    operands ops;
    lineNode *codeList = NULL;
    lineNode *dataList = NULL;
    binTree **curLabelTable = &labelTable[index];

    fp = fileArr[amOffset + index];
    if (fp == NULL)
        return -1;
    rewind(fp);

    readLine = takeInLine(currentLine.content, fp);
    while (readLine != EOF_only_line) {
        /* Keep the global lineCounter in sync for errors.c messages */
        lineCounter = lineNumber;

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

        /* Run surface-level checks (length, garbage, directive/command shape) */
        if (check_lineGeneral(currentLine.content) != 0) {
            countError++;
            lineNumber++;
            readLine = takeInLine(currentLine.content, fp);
            continue;
        }

        ptr = skipWhitespace(currentLine.content);

        /* Optional label at start of line */
        if (isLabel(ptr) == 1) {
            sscanf(ptr, "%[^:]:", label);
            currentLine.hasLabel = 1;
            strcpy(currentLine.label, label);

            nextPtr = strchr(ptr, ':');
            if (nextPtr != NULL) {
                nextPtr++;
                nextPtr = skipWhitespace(nextPtr);
                sscanf(nextPtr, "%s", first_word);
                nextPtr += (int)strlen(first_word);
                nextPtr = skipWhitespace(nextPtr);
            } else {
                countError++;
                currentLine.hasError = 1;
                strcpy(currentLine.error, "Label not followed by command");
                lineNumber++;
                readLine = takeInLine(currentLine.content, fp);
                continue;
            }
        } else {
            /* No label — read first word */
            sscanf(ptr, "%s", first_word);
            nextPtr = skipWhitespace(ptr + (int)strlen(first_word));
        }

        /* .entry */
        if (strcmp(first_word, ".entry") == 0) {
            nextPtr = skipWhitespace(ptr + (int)strlen(".entry"));
            sscanf(nextPtr, "%s", first_word);
            if (check_entryNotAlsoExterned(first_word) == ERROR)
                countError++;
            addLineNode(&entryLineArr[index], first_word, 0, lineNumber);
            lineNumber++;
            readLine = takeInLine(currentLine.content, fp);
            continue;
        }

        /* .extern */
        if (strcmp(first_word, ".extern") == 0) {
            sscanf(nextPtr, "%s", currentLine.label);
            if (check_externNotAlsoEntryed(currentLine.label) == ERROR)
                countError++;
            addLineNode(&externLineArr[index], currentLine.label, 0, lineNumber);

            if (addNode(curLabelTable, currentLine.label, 0, EXTERN, 1, 0) == ERROR)
                countError++;

            lineNumber++;
            readLine = takeInLine(currentLine.content, fp);
            continue;
        }

        /* Data directives */
        if (isData(first_word)) {
            if (currentLine.hasLabel) {
                addSymbolToData(curLabelTable, currentLine.label, DC);
            }

            if (strcmp(first_word, ".string") == 0)
                processStringDirective(nextPtr, &DC, &dataList, lineNumber);
            else if (strcmp(first_word, ".data") == 0)
                processDataDirective(nextPtr, &DC, &dataList, lineNumber);
            else if (strcmp(first_word, ".mat") == 0)
                processMatDirective(nextPtr, &DC, &dataList, lineNumber);
        }
        /* Instructions */
        else if (isInstruction(first_word)) {
            if (currentLine.hasLabel) {
                addNode(curLabelTable, currentLine.label, IC, CODE, 0, 0);
            }
            ops = parseOperands(nextPtr, first_word);
            processInstructionLine(first_word, ops, &IC, &codeList, lineNumber,
                                   *curLabelTable, &externLineArr[index]);
        }
        /* Unknown token — count as error (errors.c already printed details) */
        else {
            strcpy(currentLine.error, "Unknown command");
            currentLine.hasError = 1;
            countError++;
        }

        lineNumber++;
        readLine = takeInLine(currentLine.content, fp);
    }

    /* After we know final IC, shift DATA labels and list items by IC */
    addIC(curLabelTable, IC);
    addICList(dataList, IC);

    /* Merge code + data and store counters */
    lineArr[index] = concatLists(codeList, dataList);
    dcArr[index] = DC;
    icArr[index] = IC;

    /* Global memory limit check */
    if (IC + DC >= MAX_TOTAL_ADDRESSES) {
        printf("\nProgram size (IC + DC = %d) exceeds maximum memory limit (%d) in file \"%s\"\n\n",
               (IC + DC), MAX_TOTAL_ADDRESSES, nameArr[fileCounter]);
        countError++;
    }

    return countError;
}

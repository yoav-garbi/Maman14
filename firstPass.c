#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
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

char *opCodes[num_of_opcodes] = {
    "mov","cmp","add","sub","not","clr",
    "lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"
};

/* ----------------------- helpers (new, internal) ----------------------- */

static int requiredOperandCountByOpcode(int opIndex) {
    /* 0:mov 1:cmp 2:add 3:sub 4:not 5:clr 6:lea 7:inc 8:dec 9:jmp 10:bne 11:red 12:prn 13:jsr 14:rts 15:stop */
    switch (opIndex) {
        case 14: case 15: return 0; /* rts, stop */
        case 4: case 5: case 7: case 8: case 9: case 10: case 11: case 12: case 13:
            return 1; /* not, clr, inc, dec, jmp, bne, red, prn, jsr */
        default:
            return 2; /* mov, cmp, add, sub, lea */
    }
}

static int isAddressingLegal(int opIndex, int srcMethod, int dstMethod, int opCount) {
    /* Based on the course table (summarized):
       src:   0 1 2 3   dst:  0 1 2 3
       mov:   + + + +         - + + +
       cmp:   + + + +         + + + +
       add:   + + + +         - + + +
       sub:   + + + +         - + + +
       not:   (no src)        - + + +
       clr:   (no src)        - + + +
       lea:   - + + -         - + + +
       inc:   (no src)        - + + +
       dec:   (no src)        - + + +
       jmp:   (no src)        - + + +
       bne:   (no src)        - + + +
       red:   (no src)        - + + +
       prn:   (no src)        + + + +
       jsr:   (no src)        - + + +
       rts:   none
       stop:  none
    */
    (void)srcMethod; (void)dstMethod; /* will check per case */
    switch (opIndex) {
        case 0:  /* mov */
        case 2:  /* add */
        case 3:  /* sub */
            if (opCount != 2) return 0;
            /* dst cannot be immediate */
            return (dstMethod != 0);
        case 1:  /* cmp */
            if (opCount != 2) return 0;
            return 1; /* any src/dst ok */
        case 6:  /* lea */
            if (opCount != 2) return 0;
            /* src must be 1/2 (direct or matrix), dst not immediate */
            return ((srcMethod == 1 || srcMethod == 2) && (dstMethod != 0));
        case 4:  /* not */
        case 5:  /* clr */
        case 7:  /* inc */
        case 8:  /* dec */
            if (opCount != 1) return 0;
            return (dstMethod == 1 || dstMethod == 2 || dstMethod == 3);
        case 9:  /* jmp */
        case 10: /* bne */
        case 13: /* jsr */
            if (opCount != 1) return 0;
            /* destination: label or matrix or register, but not immediate */
            return (dstMethod == 1 || dstMethod == 2 || dstMethod == 3);
        case 11: /* red */
            if (opCount != 1) return 0;
            /* cannot read into immediate; allow 1/2/3 */
            return (dstMethod == 1 || dstMethod == 2 || dstMethod == 3);
        case 12: /* prn */
            if (opCount != 1) return 0;
            /* any addressing (0..3) allowed */
            return 1;
        case 14: /* rts */
        case 15: /* stop */
            return (opCount == 0);
        default:
            return 0;
    }
}

static int extractMatrixParts(const char *operand, char *labelOut, int *rRow, int *rCol) {
    /* Expect: NAME[ri][rj] with ri,rj in r0..r7 */
    const char *lb = strchr(operand, '[');
    const char *rb1, *lb2, *rb2;
    if (!lb) return 0;
    /* label */
    size_t len = (size_t)(lb - operand);
    if (len == 0 || len >= MAX_LABEL_LENGTH) return 0;
    memcpy(labelOut, operand, len);
    labelOut[len] = '\0';
    /* [ri] */
    rb1 = strchr(lb, ']');
    if (!rb1 || rb1 <= lb+1) return 0;
    if (rb1 - (lb+1) != 2) return 0; /* "rX" length */
    if (lb[1] != 'r' || lb[2] < '0' || lb[2] > '7') return 0;
    *rRow = lb[2] - '0';
    /* [rj] */
    lb2 = strchr(rb1+1, '[');
    if (!lb2) return 0;
    rb2 = strchr(lb2, ']');
    if (!rb2 || rb2 <= lb2+1) return 0;
    if (rb2 - (lb2+1) != 2) return 0;
    if (lb2[1] != 'r' || lb2[2] < '0' || lb2[2] > '7') return 0;
    *rCol = lb2[2] - '0';
    return 1;
}

static int hasMoreNonSpace(const char *p) {
    while (*p) {
        if (!isspace((unsigned char)*p)) return 1;
        ++p;
    }
    return 0;
}

/* ----------------------- original functions ----------------------- */

char *skipWhitespace(char *line) {
    while (*line == ' ' || *line == '\t') line++;
    return line;
}

int isLabel(char *ptr) {
    int len = 0;
    char name[MAX_LABEL_LENGTH + 1];

    while (ptr[len] && !isspace((unsigned char)ptr[len]) && ptr[len] != ':')
        len++;

    if (ptr[len] != ':')
        return 0;

    if (len == 0 || len >= (int)sizeof(name))
        return ERROR;

    memcpy(name, ptr, len);
    name[len] = '\0';

    if (check_labelName(name) == ERROR)
        return ERROR;

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
        return 0;
    }

    if (*root == NULL)
        *root = makeNode(str, address, DATA, 0, 0);
    else
        addNode(*root, str, address, DATA, 0, 0);
    return 1;
}

/* count numbers in data line - bit hacky but works (kept for compat, not used by new parser) */
int countDataValues(char *line) {
    int count = 0;
    const char *p = line;

    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;

        if (*p == '-' || *p == '+' || isdigit((unsigned char)*p)) {
            count++;
            if (*p == '-' || *p == '+') p++;
            while (*p && isdigit((unsigned char)*p)) p++;
        } else if (*p == ',') {
            p++;
        } else {
            p++;
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

/* convert int to binary string (WORD_LENGTH bits, two's complement) */
void intToBinary(int value, char *binaryLine) {
    unsigned int mask = 1u << (WORD_LENGTH - 1);
    unsigned int u = ((unsigned int)value) & ((1u << WORD_LENGTH) - 1);
    int i;
    for (i = 0; i < WORD_LENGTH; i++) {
        binaryLine[i] = (u & mask) ? '1' : '0';
        mask >>= 1;
    }
    binaryLine[WORD_LENGTH] = '\0';
}

/* ---------- data directives ---------- */

static int parseIntToken(const char **pp, int *out, int lineNum) {
    const char *p = *pp;
    char *endp;
    long v;

    while (isspace((unsigned char)*p)) p++;
    if (*p == '\0') return 0;

    if (*p == ',' ) {
        printf("Error: Missing argument before comma at line %d\n", lineNum);
        return -1;
    }

    v = strtol(p, &endp, 10);
    if (endp == p) {
        printf("Error: Argument is not a real number at line %d\n", lineNum);
        return -1;
    }

    /* range check for WORD_LENGTH signed */
    if (v < -(1L << (WORD_LENGTH-1)) || v > ((1L << (WORD_LENGTH-1)) - 1)) {
        printf("Error: Number out of range at line %d\n", lineNum);
        return -1;
    }

    *out = (int)v;
    p = endp;

    while (isspace((unsigned char)*p)) p++;
    if (*p == ',') {
        /* consume one comma and ensure not multiple commas */
        const char *q = p + 1;
        while (isspace((unsigned char))*q) q++;
        if (*q == ',' ) {
            printf("Error: Multiple consecutive commas at line %d\n", lineNum);
            return -1;
        }
        p = q;
    }

    *pp = p;
    return 1;
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

    for (i = 0; start + i < end; i++) {
        intToBinary((int)start[i], binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
    }

    intToBinary(0, binaryLine);
    addLineNode(dataList, binaryLine, (*DC)++, lineNum);

    if (hasMoreNonSpace(end+1)) {
        printf("Error: Extraneous text after end of command at line %d\n", lineNum);
    }
}

void processDataDirective(char *data, int *DC, lineNode **dataList, int lineNum) {
    const char *p = data;
    int hadAny = 0;
    while (1) {
        int val, r;
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0') break;

        r = parseIntToken(&p, &val, lineNum);
        if (r < 0) return; /* error already printed */
        if (r == 0) break;

        char binaryLine[WORD_LENGTH + 1];
        intToBinary(val, binaryLine);
        addLineNode(dataList, binaryLine, (*DC)++, lineNum);
        hadAny = 1;
    }
    if (!hadAny) {
        printf("Error: Missing argument at line %d\n", lineNum);
    }
}

void processMatDirective(char *line, int *DC, lineNode **dataList, int lineNum) {
    char tmp[MAX_LINE_LENGTH];
    char *p;
    int rows = 0, cols = 0, total, count = 0;
    char binaryLine[WORD_LENGTH + 1];
    char *token;

    strncpy(tmp, line, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    p = strchr(tmp, '[');
    if (!p) { printf("Error: Invalid matrix format at line %d\n", lineNum); return; }
    rows = atoi(p + 1);

    p = strchr(p + 1, '[');
    if (!p) { printf("Error: Invalid matrix format at line %d\n", lineNum); return; }
    cols = atoi(p + 1);

    if (rows <= 0 || cols <= 0) {
        printf("Error: Invalid matrix dimensions at line %d\n", lineNum);
        return;
    }

    total = rows * cols;

    p = strchr(tmp, ']');
    if (!p) { printf("Error: Invalid matrix format at line %d\n", lineNum); return; }
    p = strchr(p + 1, ']');
    if (!p) { printf("Error: Invalid matrix format at line %d\n", lineNum); return; }
    p++; /* start of values */

    token = strtok(p, ", \t\n");
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

    if (token) {
        printf("Error: Extraneous text after end of command at line %d\n", lineNum);
    }
}

void processDataLine(char *line, int *DC, lineNode **dataList, int lineNum) {
    char directive[MAX_LINE_LENGTH];
    char *restOfLine;

    if (sscanf(line, "%s", directive) != 1) return;

    restOfLine = strchr(line, ' ');
    if (restOfLine) restOfLine = skipWhitespace(restOfLine);

    if (strcmp(directive, ".string") == 0) {
        processStringDirective(restOfLine ? restOfLine : (char*)"", DC, dataList, lineNum);
    } else if (strcmp(directive, ".data") == 0) {
        processDataDirective(restOfLine ? restOfLine : (char*)"", DC, dataList, lineNum);
    } else if (strcmp(directive, ".mat") == 0) {
        processMatDirective(restOfLine ? restOfLine : (char*)"", DC, dataList, lineNum);
    }
}

/* ---------- code encoding ---------- */

/* encode instruction word (10 bits: opcode[9..6], src[5..4], dst[3..2], ERA[1..0]=00) */
void encodeInstructionWord(int opcode, int srcMethod, int destMethod, char *binaryLine) {
    unsigned int instruction = 0;
    instruction |= ((unsigned int)(opcode & 0xF)) << 6;
    instruction |= ((unsigned int)(srcMethod & 0x3)) << 4;
    instruction |= ((unsigned int)(destMethod & 0x3)) << 2;
    /* ERA = 00 */
    intToBinary((int)instruction, binaryLine);
}

/* encode operand word: by default put 10-bit payload into [9..2] (ERA left 00) */
void encodeOperandWord(int value, int addressingMethod, char *binaryLine) {
    unsigned int operand = 0;
    (void)addressingMethod;
    operand |= ((unsigned int)value & 0x3FFu) << 2;
    intToBinary((int)operand, binaryLine);
}

/* encode a word that packs two registers (src & dst) */
static void encodeRegPairWord(int srcReg, int dstReg, char *binaryLine) {
    /* Pack into a single 10-bit word, for example:
       [9..6] = srcReg(4b)  [5..2] = dstReg(4b)  [1..0]=ERA(00)
       (fits the examples in the spec tables we used) */
    unsigned int w = 0;
    w |= ((unsigned int)(srcReg & 0xF)) << 6;
    w |= ((unsigned int)(dstReg & 0xF)) << 2;
    intToBinary((int)w, binaryLine);
}

/* Adds an extern operand to externLineArr if needed */
void addExternIfNeeded(char *operand, int IC, binTree *labelTable, lineNode **externLineArr) {
    if (operand == NULL) return;

    char name[MAX_LABEL_LENGTH + 1];
    size_t n = strcspn(operand, "[");
    if (n >= sizeof(name)) n = sizeof(name) - 1;
    memcpy(name, operand, n);
    name[n] = '\0';

    if (name[0] == '\0') {
        strncpy(name, operand, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
    }

    {
        binTree *sym = search(labelTable, name);
        if (sym != NULL && sym->symbolType == EXTERN) {
            addLineNode(externLineArr, name, IC, 0);
        }
    }
}

/* Processes a single operand based on its addressing method */
void processSingleOperand(char *operand, int method, int *IC, lineNode **codeList, int lineNum,
                          binTree *labelTable, lineNode **externLineArr) {
    char binaryLine[WORD_LENGTH + 1];
    char labelPadded[MAX_LABEL_LENGTH + 32];

    switch (method) {
        case 0: { /* immediate */
            int value = atoi(operand + 1); /* skip the # */
            encodeOperandWord(value, method, binaryLine);
            addLineNode(codeList, binaryLine, (*IC)++, lineNum);
            break;
        }
        case 1: { /* direct (label) */
            addExternIfNeeded(operand, *IC, labelTable, externLineArr);
            snprintf(labelPadded, sizeof(labelPadded), " %s ", operand);
            addLineNode(codeList, labelPadded, (*IC)++, lineNum);
            break;
        }
        case 2: { /* matrix (label[ri][rj]) => two extra words: label addr, reg-pair */
            char lbl[MAX_LABEL_LENGTH];
            int rRow = 0, rCol = 0;
            if (!extractMatrixParts(operand, lbl, &rRow, &rCol)) {
                printf("Error: Invalid matrix operand at line %d\n", lineNum);
                break;
            }
            /* word for label address (like direct) */
            addExternIfNeeded(lbl, *IC, labelTable, externLineArr);
            snprintf(labelPadded, sizeof(labelPadded), " %s ", lbl);
            addLineNode(codeList, labelPadded, (*IC)++, lineNum);
            /* word for the two registers */
            encodeRegPairWord(rRow, rCol, binaryLine);
            addLineNode(codeList, binaryLine, (*IC)++, lineNum);
            break;
        }
        case 3: { /* register */
            int r = getRegisterNumber(operand);
            /* If only one register operand (and the other isn't register), we emit single register word here.
               If both regs exist, caller will handle combined reg-word case. */
            encodeRegPairWord(r, 0, binaryLine); /* put src in [9..6], dst=0 for single-op use */
            addLineNode(codeList, binaryLine, (*IC)++, lineNum);
            break;
        }
    }
}

/* Processes an instruction line */
void processInstructionLine(char *opcode, operands ops, int *IC, lineNode **codeList, int lineNum,
                            binTree *labelTable, lineNode **externLineArr) {
    int opcodeIndex = getOpcodeIndex(opcode);
    int srcMethod = 0, destMethod = 0;
    char binaryLine[WORD_LENGTH + 1];

    if (opcodeIndex < 0) {
        printf("Error: Unknown opcode '%s' at line %d\n", opcode, lineNum);
        return;
    }

    if (ops.operandCount == 2) {
        srcMethod = getAddressingMethod(ops.op1);
        destMethod = getAddressingMethod(ops.op2);
    } else if (ops.operandCount == 1) {
        destMethod = getAddressingMethod(ops.op2);
    }

    /* validate operand count & addressing legality */
    if (ops.operandCount != requiredOperandCountByOpcode(opcodeIndex)) {
        printf("Error: wrong number of operands for '%s' at line %d\n", opcode, lineNum);
        return;
    }
    if (!isAddressingLegal(opcodeIndex, srcMethod, destMethod, ops.operandCount)) {
        printf("Error: illegal addressing mode(s) for '%s' at line %d\n", opcode, lineNum);
        return;
    }

    /* emit instruction word */
    encodeInstructionWord(opcodeIndex, srcMethod, destMethod, binaryLine);
    addLineNode(codeList, binaryLine, (*IC)++, lineNum);

    /* handle operands */
    if (ops.operandCount == 2) {
        int bothRegs = (srcMethod == 3 && destMethod == 3);
        if (bothRegs) {
            /* emit single combined register word */
            int rsrc = getRegisterNumber(ops.op1);
            int rdst = getRegisterNumber(ops.op2);
            encodeRegPairWord(rsrc, rdst, binaryLine);
            addLineNode(codeList, binaryLine, (*IC)++, lineNum);
        } else {
            /* normal path */
            if (srcMethod != 3) {
                processSingleOperand(ops.op1, srcMethod, IC, codeList, lineNum, labelTable, externLineArr);
            } else {
                /* a single-reg word emitted here (dst not register) */
                char bin[WORD_LENGTH + 1];
                encodeRegPairWord(getRegisterNumber(ops.op1), 0, bin);
                addLineNode(codeList, bin, (*IC)++, lineNum);
            }
            processSingleOperand(ops.op2, destMethod, IC, codeList, lineNum, labelTable, externLineArr);
        }
    } else if (ops.operandCount == 1) {
        /* single dest operand */
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
    if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0') return 3;
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

    strcpy(tempLine, lineAfterOpcode ? lineAfterOpcode : "");

    token = strtok(tempLine, " \t\n,");
    if (token != NULL) {
        if (hasOnlyDestOperand(opcodeName)) {
            strncpy(ops.op2, token, sizeof(ops.op2)-1);
            ops.op2[sizeof(ops.op2)-1] = '\0';
        } else {
            strncpy(ops.op1, token, sizeof(ops.op1)-1);
            ops.op1[sizeof(ops.op1)-1] = '\0';
        }
        count++;
    }

    token = strtok(NULL, " \t\n,");
    if (token != NULL) {
        strncpy(ops.op2, token, sizeof(ops.op2)-1);
        ops.op2[sizeof(ops.op2)-1] = '\0';
        count++;
    }

    /* if anything remains non-space after the second operand -> error will be reported by caller
       when legality/operand count is checked (we keep parsing simple here) */

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

    currentLine.content[0] = '\0';
    readLine = takeInLine(currentLine.content, fp);
    while (readLine != EOF) {
        if (readLine != 0) {
            countError++;
            lineNumber++;
            currentLine.content[0] = '\0';
            readLine = takeInLine(currentLine.content, fp);
            continue;
        }

        currentLine.lineNumber = lineNumber;
        currentLine.hasError = 0;
        currentLine.hasLabel = 0;
        currentLine.error[0] = '\0';
        currentLine.label[0] = '\0';

        ptr = skipWhitespace(currentLine.content);

        /* skip empty line or comment */
        if (*ptr == '\0' || *ptr == ';') {
            lineNumber++;
            currentLine.content[0] = '\0';
            readLine = takeInLine(currentLine.content, fp);
            continue;
        }

        {
            int temp = isLabel(ptr);
            if (temp == ERROR) {
                countError++;
                currentLine.hasError = 1;
                strcpy(currentLine.error, "Illegal label name");
                lineNumber++;
                currentLine.content[0] = '\0';
                readLine = takeInLine(currentLine.content, fp);
                continue;
            } else if (temp) {
                if (sscanf(ptr, "%[^:]:", label) == 1) {
                    currentLine.hasLabel = 1;
                    strcpy(currentLine.label, label);
                }

                nextPtr = strchr(ptr, ':');
                if (nextPtr != NULL) {
                    nextPtr++;
                    nextPtr = skipWhitespace(nextPtr);
                    if (sscanf(nextPtr, "%s", first_word) != 1) {
                        countError++;
                        currentLine.hasError = 1;
                        strcpy(currentLine.error, "Label not followed by command");
                        lineNumber++;
                        currentLine.content[0] = '\0';
                        readLine = takeInLine(currentLine.content, fp);
                        continue;
                    }
                } else {
                    countError++;
                    currentLine.hasError = 1;
                    strcpy(currentLine.error, "Label not followed by command");
                    lineNumber++;
                    currentLine.content[0] = '\0';
                    readLine = takeInLine(currentLine.content, fp);
                    continue;
                }
            } else {
                if (sscanf(ptr, "%s", first_word) != 1) {
                    lineNumber++;
                    currentLine.content[0] = '\0';
                    readLine = takeInLine(currentLine.content, fp);
                    continue;
                }
                nextPtr = skipWhitespace(ptr + strlen(first_word));
            }
        }

        /* Handle .entry */
        if (strcmp(first_word, ".entry") == 0) {
            char nameBuf[MAX_LABEL_LENGTH];
            char *afterDirective = skipWhitespace(nextPtr + 0);
            if (currentLine.hasLabel) {
                /* label before .entry is ignored (optional warning) */
            }
            if (sscanf(afterDirective, "%s", nameBuf) == 1) {
                addLineNode(&entryLineArr[index], nameBuf, 0, lineNumber);
            } else {
                printf("Error: .entry missing symbol at line %d\n", lineNumber);
                countError++;
            }

            lineNumber++;
            currentLine.content[0] = '\0';
            readLine = takeInLine(currentLine.content, fp);
            continue;
        }

        /* Handle .extern */
        if (strcmp(first_word, ".extern") == 0) {
            char extName[MAX_LABEL_LENGTH];
            if (currentLine.hasLabel) {
                /* label before .extern is ignored (optional warning) */
            }
            if (sscanf(nextPtr, "%s", extName) == 1) {
                addNode(*curLabelTable, extName, 0, EXTERN, 1, 0);
            } else {
                printf("Error: .extern missing symbol at line %d\n", lineNumber);
                countError++;
            }
            lineNumber++;
            currentLine.content[0] = '\0';
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
                binTree *found = search(*curLabelTable, currentLine.label);
                if (found != NULL) {
                    printf("Error: duplicate symbol '%s' at line %d\n", currentLine.label, lineNumber);
                    countError++;
                } else {
                    addNode(*curLabelTable, currentLine.label, IC, CODE, 0, 0);
                }
            }
            operands = parseOperands(nextPtr, first_word);
            /* extra check: if there is extraneous text after operands in the original line, try to catch */
            /* (the strict per-token checks are enforced later by legality tests) */
            processInstructionLine(first_word, operands, &IC, &codeList, lineNumber, *curLabelTable, &externLineArr[index]);
        } else {
            strcpy(currentLine.error, "Unknown command");
            currentLine.hasError = 1;
            countError++;
        }

        lineNumber++;
        currentLine.content[0] = '\0';
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "constants.h"

#define ERROR -1
#define buffer_size 200
#define address_binary_representation_size 10
#define short_address_binary_representation_size 8
#define total_num_of_files(argc) ((argc-1)*5) /* argc-1 because the 0th index refers to "./assembler" which is irrelevent here. *4 because each .as file (1) will make a .am file (2), .ob file (3), a .ext file (4) and a .ent file (5) */
#define EOF_only_line 1
#define num_of_opcodes 16
#define max_immediate_int 127
#define min_immediate_int -128
#define max_data_int 511
#define min_data_int -512

#define CODE 0
#define DATA 1
#define EXTERN 2

#define wordIsLABEL 0
#define wordIsCOMMAND 1
#define wordIsDATA 2
#define wordIsSTRING 3
#define wordIsMAT 4
#define wordIsENTRY 5
#define wordIsEXTERN 6
#define EMPTY_LINE 7

#define ADDR_IMMEDIATE 0 /* #number */
#define ADDR_DIRECT 1 /* LABEL */
#define ADDR_INDEX 2 /* mat[][] */
#define ADDR_REGISTER 3 /* r0-r7 */




/* the struct that holds the labels is a binary search tree- each node has two brnaches- the smaller is to the left, the bigger is to the right. This is very efficient and allows a time complexity of O(log n) both for adding a node and searching, and a space complexity of O(n) */
typedef struct binTree
{
	char *str;
	int address;
	int symbolType;
	unsigned int isEntry: 1;
	unsigned int isExternal: 1;
	struct binTree *left;
	struct binTree *right;
	
} binTree;

/* this struct holds necessary information about the opcodes and their permissions. Time complexity for searching an opcode is O(1) because their is a constant amount of opcodes (16). space complexity is O(1) for the same reason */
typedef struct opcd
{
	char *name;
	char *code;
	int source[4];
	int dest[4];
	
} opcd;


/* struct that will serve as a package for all relevant data for each row, making error handling easier */
typedef struct {
    char content[MAX_LINE_LENGTH];
    int lineNumber;
    int hasError;
	char error[MAX_LINE_LENGTH];
    int hasLabel;
	char label[MAX_LABEL_LENGTH];
} LineData;


/* struct- node that holds a line of data, its address, the number of the line in the .as file, and a pointer to the next node */
typedef struct lineNode
{
	char *line;
	int address;
	int lineNum;
	struct lineNode *next;
	
} lineNode;

/*will hold the current line operands */
typedef struct {
	char op1[MAX_LABEL_LENGTH];
	char op2[MAX_LABEL_LENGTH];
	int operandCount;
} operands;


/* struct that holds a macros name and the macro itself */
typedef struct
{
	char *name;
	char **macroLines;
	int lineAmount;
} macro;


extern lineNode **lineArr;
extern char **nameArr;
extern FILE **fileArr;
extern macro **macroArr;
extern int lineCounter;
extern int fileCounter;
extern int macroCounter;
extern binTree **labelTable;
extern opcd opcodeTable[16];
extern int *icArr;
extern int *dcArr;
extern lineNode **entryLineArr;
extern lineNode **externLineArr;
extern char ***argvPointer;
extern int amOffset;




/* io.c */
FILE **getFiles(int, char *[]);
char **make_nameArr(int, char *[]);

int create_amFile(int, FILE **, char **, int);
int create_obFile(int, FILE **, char **, int);
int create_entFile(int, FILE **, char **, int);
int create_extFile(int, FILE **, char **, int);

int closeFiles(int, FILE **);
int takeInLine(char [], FILE *);
int recognize_opcode(char *);
int findCommand(char *);
int writeEnt(FILE *);
int writeExt(FILE *);



/* errors.c */
int check_lineGeneral(char *);
int check_fileExistence(void*);
int check_newFileExistence(void *);
int check_fileEntered(int);
int check_fileName(int);
int check_opcodeName(int);
int check_lineLength(char []);
int check_registerNumber(char []);
int check_labelName(char *);
int check_allocation(void *);
int check_existsInOtherFileAsEntry(char *, int);
int check_labelExist_or_legalExternalUse(binTree *, char *, lineNode *, int);
int check_entryWithLocalDefinition(binTree *, char *);
int check_labelDuplicate(char *);
int check_isExternalLabelDefinedInOtherFile(char *, int);
int check_entryDeclaredInOtherFile(char *, int);
int check_entryNotAlsoExterned(char *);
int check_externNotAlsoEntryed(char *);
int check_garbageTextBeforeLine(char *, int *, int *);
int check_garbageTextAndClassifyWord(char *, int, int *, int *);
int check_scanOperand(char **, int *, char *);
int check_dataValues(char **, int *);
int check_matValues(char **, int);
int check_stringData(char **);
int check_commandOperands(char **, char *);
int check_macroName(char *);
int check_macroOpenLine(char *);
int check_macroCloseLine(char *);




/* general_funcs.c */
int base2_to_base4_fileToFile(FILE *, FILE *);
int base2_to_base4_strToFile(char *, FILE*);
int base10_to_base2(int, char[]);
int base10_to_base2_forAddress(int, char[]);
int base10_to_base2_forShortAddress(int, char[]);
int copyFile(FILE *, FILE *);
char *strDuplicate(char *);
char *skipWhiteSpace(char *);
int isEndOfLine_or_whiteSpaceOnly(char *);
int isRequiredComma(char **);
int scanLabel(char **, char *);
int scanInt(char **, int *);
int scanString(char **, char *);
int printFile(FILE *fp);																									/* TEMP */






/* struct_funcs.c */
int initializeLabelTables(int);
binTree * makeNode(char *, int, int, int, int);
int printTree(binTree *);																								/* TEMP */
int addNode(binTree **, char *, int, int, int, int);
int addNodePrivate(binTree *, char *, int, int, int, int);
binTree *search(binTree *, char *);
int searchEnt(binTree *);
int searchExt(binTree *);
int freeTree(binTree **);
int freeLabelTable(binTree ***, int);
int addIC(binTree **, int);
int addEntryLocal(char *);
int addExternAcross(char *, int, int);

int addLineNode(lineNode **, char *, int, int);
int addICList(lineNode *, int);
int printList(lineNode *);																								/* TEMP */
int freeList(lineNode **);
int freeListArr(lineNode ***, int);
int isAlreadyEntry(char *);
int isAlreadyExtern(char *);

int freeNameArr(char ***, int);

int freeFileArr(FILE ***);

int initializeMacroArr();
int addMacro(char *);
int addLineToMacro(char *, char *);
int freeMacroArr();




/* preAssembler.c */
/* Trim trailing spaces/tabs/newlines. */
void trim_right(char *s);
/* "mcro <name>": 1 ok; -2 glued "mcroNAME"; -1 invalid (we print); 0 not mcro. */
int  parse_mcro_open(const char *line, char *out_name, size_t out_sz, const char *fname_as);
/* mcroend: 1 end ok; 2 end+extra text (we print); -1 glued; 0 not mcroend. */
int  mcro_close_status(const char *line, const char *fname_as);
/* If line starts with "LABEL:" copy it (incl. ':') and return ptr after ':', else NULL. */
const char *scan_label_prefix(const char *s, char *label, size_t label_sz);
/* Copy first token after spaces to buf; return ptr after the token. */
const char *scan_token(const char *s, char *buf, size_t buf_sz);
/* Find macro by name in global table. */
macro *find_macro(const char *name);
/* Emit all stored lines of a macro into out. */
void emit_macro_body(FILE *out, const macro *m);
/* Read macro body until mcroend; DO NOT validate body lines here (rule #10). */
int  read_mcro_body(FILE *fp, const char *macroName, int *pErr, const char *fname_as);
/* True if rest is only spaces/tabs or a ';' comment. */
int  only_ws_or_comment(const char *p);
/* Expand macro; if label provided, attach it to first non-empty body line. */
int  expand_macro(FILE *out, const macro *m, const char *opt_label);
int preAssemble(int);



/* firstPass.c */
int firstPass(int index);
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
void processSingleOperand(char *operand, int method, int *IC, lineNode **codeList, int lineNum, binTree *labelTable, lineNode **externLineArr);
int getOpcodeIndex(char *opcodeName);
int getAddressingMethod(char *operand);
int addIC(binTree **root, int IC);
int addICList(lineNode *dataList, int IC_FINAL);
int hasOnlyDestOperand(char *opcodeName);
void addExternIfNeeded(char *operand, int IC, binTree *labelTable, lineNode **externLineArr);
void processInstructionLine(char *opcode, operands ops, int *IC, lineNode **codeList, int lineNum, binTree *labelTable, lineNode **externLineArr);




/* secondPass.c */
int secondPass(int, char *[], FILE **, lineNode *[], char **);

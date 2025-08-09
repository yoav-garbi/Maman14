#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "constants.h"

#define ERROR -1
#define buffer_size 85
#define address_binary_representation_size 8
#define total_num_of_files(argc) ((argc-1)*5) /* argc-1 because the 0th index refers to "./assembler" which is irrelevent here. *4 because each .as file (1) will make a .am file (2), .ob file (3), a .ext file (4) and a .ent file (5) */
#define EOF_only_line 1
#define num_of_opcodes 16
#define max_immediate_int 127
#define min_immediate_int -128

#define CODE 0
#define DATA 1

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



extern lineNode **lineArr;
extern char **nameArr;
extern FILE **fileArr;
extern int lineCounter;
extern int fileCounter;
extern binTree **labelTable;
extern opcd opcodeTable[16];
extern int *icArr;
extern int *dcArr;
extern lineNode **entryLineArr;
extern lineNode **externLineArr;



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
int writeEnt(FILE *, binTree *);
int writeExt(FILE *, binTree *);



/* errors.c */
int check_lineGeneral(char *);
int check_fileExistence(void*);
int check_newFileExistence(void *);
int check_fileEntered(int);
int check_fileName(int, char *[]);
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
int check_garbageTextBeforeLine(char *, int *, int *);
int check_garbageTextAndClassifyWord(char *, int, int *, int *);
int check_scanOperand(char **, int *, char *);
int check_dataValues(char **, int *);
int check_matValues(char **, int);
int check_stringData(char **);
int check_commandOperands(char **, char *);





/* general_funcs.c */
int base2_to_base4_fileToFile(FILE *, FILE *);
int base2_to_base4_strToFile(char *, FILE*);
int base10_to_base2(int, char[]);
int base10_to_base2_forAddress(int, char[]);
int copyFile(FILE *, FILE *);
char *strDuplicate(char *);
char *skipWhiteSpace(char *);
int isEndOfLine_or_whiteSpaceOnly(char *);
char *skipWhiteSpace(char *);
int isEndOfLine_or_whiteSpaceOnly(char *);
int isRequiredComma(char **);
int scanLabel(char **, char *);
int scanInt(char **, int *);
int scanString(char **, char *);






/* struct_funcs.c */
int initializeLabelTables(int);
binTree * makeNode(char *, int, int, int, int);
int setL(binTree *, binTree *);
int setR(binTree *, binTree *);
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
int printList(lineNode *);																								/* TEMP */
int freeList(lineNode **);
int freeListArr(lineNode ***, int);
int isAlreadyEntry(char *);
int isAlreadyExtern(char *);


int freeNameArr(char ***, int);

int freeFileArr(FILE ***);




/* secondPass.c */
int secondPass(int, char *[], FILE **, lineNode *[], char **);

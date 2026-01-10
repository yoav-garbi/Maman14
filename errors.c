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

#define CODE 0
#define DATE 1
#define MAT 2
#define EXTERN 3
#define ENTRY 4

/* the struct that holds the labels is a binary search tree- each node has two brnaches- the smaller is to the left, the bigger is to the right. This is very efficient and allows a time complexity of O(log n) both for adding a node and searching, and a space complexity of O(n) */
typedef struct binTree
{
	char *str;
	int address;
	int symbolType;
	int isEntry: 1;
	int isExternal: 1;
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
} LineDate;


/* struct that holds a line of data (in binary) and its address */
typedef struct lineNode
{
	char *line;
	int address;
	struct lineNode *next;
	
} lineNode;


extern int lineCounter;
extern binTree *labelTable;
extern opcd opcodeTable[16];


/* io.c */
FILE **getFiles(int, char *[]);
char **make_nameArr(int, char *[]);

int create_amFile(int, FILE **, char **, int);
int create_obFile(int, FILE **, char **, int);
int create_entFile(int, FILE **, char **, int);
int create_extFile(int, FILE **, char **, int);

int closeFiles(int, FILE **);
int takeInLine(char [], FILE *);
int skipNotesAndWhiteLines(char [], FILE *);
int recognize_opcode(char *);
int findCommand(char *);
int writeEnt(FILE *, binTree *);
int writeExt(FILE *, binTree *);



/* errors.c */
int check_lineGeneral(char *);
int check_fileExistence(void*);
int check_newFileExistence(void *);
int check_fileEntered(int);
int check_opcodeName(int);
int check_legalAddressing(int, int, int);
int check_lineLength(char []);
int check_registerNumber(char []);
int check_labelLength(char []);
int check_labelName(char *, int);
int check_allocation(void *);
int check_labelExist(binTree *);



/* general_funcs.c */
int base2_to_base4_fileToFile(FILE *, FILE *);
int base2_to_base4_strToFile(char *, FILE*);
int base10_to_base2(int, char[]);
int base10_to_base2_forAddress(int, char[]);
int copyFile(FILE *, FILE *);




/* struct_funcs.c */
binTree * makeNode(char *, int, int, int, int);
int setL(binTree *, binTree *);
int setR(binTree *, binTree *);
int printTree(binTree *);																								/* TEMP */
int addNode(binTree **, char *, int, int, int, int);
int addNodePrivate(binTree *, char *, int, int, int, int);
binTree *search(binTree *, char *);
int searchEnt(binTree *);
int searchExt(binTree *);

int addLineNode(lineNode **, char *, int);
int printList(lineNode *);																								/* TEMP */



/* secondPass.c */
int secondPass(int, char *[], FILE **, lineNode *[], char **);











																																		/* TEMP */
int isLabel(const char *ptr);

#define ERROR -1
#define buffer_size 100
#define binary_representation_size 9
#define total_num_of_files(argc) ((argc)*2+2) /* argc*2 because each as file will make a new ob file. +2 because there are an additional two file- ext and ent */

/* the struct that holds the labels is a binary search tree- each node has two brnaches- the smaller is to the left, the bigger is to the right. This is very efficient and allows a time complexity of O(log n) both for adding a node and searching, and a space complexity of O(n) */
typedef struct binTree
{
	char *str;
	int address;
	struct binTree *left;
	struct binTree *right;
	
} binTree;

/* struct that will serve as a package for all relevant data for each row, making error handling easier */
typedef struct {
    char content[MAX_LINE_LENGTH];
    int lineNumber;
    int hasError;
    
} LineDate;


/* this struct holds necessary information about the opcodes and their permissions. Time complexity for searching an opcode is O(1) because their is a constant amount of opcodes (16). space complexity is O(1) for the same reason */
typedef struct opcd
{
	char *name;
	char *code;
	int source[4];
	int dest[4];
	
} opcd;



/* io.c */
FILE **getFiles(int, char *[]);
int closeFiles(int argc, FILE **fileArr);



/* errors.c */
int checkFileExistence(void*);



/* general_funcs.c */
int base2_to_base4(void*, void*);
int base10_to_base2(int, char[]);



/* binTree_funcs.c */
binTree * makeNode(char *, int);
int setL(binTree *, binTree *);
int setR(binTree *, binTree *);
int printTree(binTree *);																								/* TEMP */
int addNode(binTree *, char *, int);
binTree * search(binTree *, char *);

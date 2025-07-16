#define ERROR -1
#define buffer_size 100

/* the struct that holds the labels is a binary search tree- each node has two brnaches- the smaller is to the left, the bigger is to the right. This is very efficient and allows a time complexity of O(log n) both for adding a node and searching, and a space complexity of O(n) */
typedef struct binTree
{
	char *str;
	int address;
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


/* errors.c */
int checkFileExistence(void*);



/* general_funcs.c */
int base2_to_base4(void*, void*);



/* binTree_funcs.c */
binTree * makeNode(char *, int);
int setL(binTree *, binTree *);
int setR(binTree *, binTree *);
int printTree(binTree *);																								/* TEMP */
int addNode(binTree *, char *, int);
binTree * search(binTree *, char *);

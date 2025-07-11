#define ERROR -1
#define buffer_size 100

typedef struct
{
	char *str;
	int address;
	struct binTree *left;
	struct binTree *right;
	
} binTree;


/* errors.c */
int checkFileExistence(void*);



/* general_funcs.c */
int base2_to_base4(void*, void*);



/* binTree_funcs.c */
binTree * makeNode(char *, int);
int setL(binTree *, binTree *);
int setR(binTree *, binTree *);
int printTree(binTree *);																								/* TEMP */

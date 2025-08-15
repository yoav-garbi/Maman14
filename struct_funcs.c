#include "prototypes.h"



int initializeLabelTables(int argc)
{
	/* create an array of labelTables- each cell points to a binTree that acts as a labelTable for one file */
	labelTable = calloc(argc - 1, sizeof(binTree *));
	if (check_allocation(labelTable) == ERROR)
		return ERROR;
	
	return 0;
}



/* creates a node and fills it with the data entered (branches are set to NULL) */
binTree * makeNode(char *str, int address, int type, int external, int entry)
{
	binTree *node = malloc(sizeof(binTree));
	if (node == NULL)
		return NULL;
	
	node->str = strDuplicate(str);
	node->address = address;
	node->symbolType = type;
	node->isExternal = external;
	node->isEntry = entry;
	node->left = NULL;
	node->right = NULL;
	
	return node;
}

/* allows to change the pointer to the left */
int setL(binTree **root, binTree *left)
{
	(*root)->left = left;
	
	return 0;
}

/* allows to change the pointer to the right */
int setR(binTree **root, binTree *right)
{
	(*root)->right = right;
	
	return 0;
}

/* prints the tree (in order) */
int printTree(binTree *root)																								/* TEMP */
{
	if (root == NULL)
		return 0;
	
	printTree(root->left);
	printf("str: %s\taddress: %d\tsymbolType: %d\tisExternal: %d\tisEntry: %d\n", root->str, root->address, root->symbolType, root->isExternal, root->isEntry);
	printTree(root->right);
	
	return 0;
}


/* adds a node to the correct place in the tree. time compexity = O(log n) because every level cuts down the number of option by half. space complexity = O(n)- there is no waste of space because nodes are initiallized as needed and not ahead of time. Each node is O(n) because of the string but that is out of out control. In this prohect we will look at a node as a single unit so the space complexity of each node is O(1) 
The correct place in the tree is determined with the help of strcmp (from <string.h>) that compares 2 string an returns if they are identical or if one has a bigger value (lexicographically) */
int addNode(binTree **root, char *str, int address, int type, int external, int entry)
{
	binTree *temp;
	
	if (*root == NULL)
	{
		*root = makeNode(str, address, type, external, entry);
		return 0;
	}
	
	temp = search(*root, str);
	/* this label is already in this binTree (as a placeholder- it isn't complete yet) because it was passed as an entry from another file. The fact that the label isn't external yet means that it hasn't been added to this binTree from it's own source file yet, so we can add it now */
	if (temp != NULL && temp->isEntry && !temp->isExternal)
	{
		temp->address = address;
		temp->isExternal = 1;
		return 0;
	}
	
	/* check for duplicate definition in same file- two labels declared in the same file with the same name */
	if (temp != NULL)
	{
		check_labelDuplicate(str);
		return ERROR;
	}
	
	/* completely new label (not in the tree at all yet) */
	addNodePrivate(*root, str, address, type, external, entry);
	return 0;
}


int addNodePrivate(binTree *root, char *str, int address, int type, int external, int entry)
{
	int comp;
	
	if (root == NULL)
		return 0;
	
	comp = strcmp(str, root->str);
	
	
	if (comp < 0)
	{
		if (root->left == NULL)
		{
			setL(&root, makeNode(str, address, type, external, entry));
			return 0;
		}
		
		addNodePrivate(root->left, str, address, type, external, entry); /* use recursion to decend 1 more level */
	}
	
	else if (comp > 0)
	{
		if (root->right == NULL)
		{
			setR(&root, makeNode(str, address, type, external, entry));
			return 0;
		}
		
		addNodePrivate(root->right, str, address, type, external, entry); /* use recursion to decend 1 more level */
	}
	
	else /* this label already exists, and isn't an entry */
	{
		check_labelDuplicate(str);
		return ERROR;
	}
	
	return 0;
}



/* searches a node (a string) in the tree. time compexity = O(log n) because every level cuts down the number of option by half.
If the node in question doesnt't exist, the funcion returns NULL  */
binTree * search(binTree *root, char *str)
{
	int comp;
	
	if (root == NULL)
		return NULL;
	
	comp = strcmp(str, root->str);
	
	
	if (comp < 0)
	{
		if (root->left == NULL)
			return NULL;
		
		return search(root->left, str); /* use recursion to decend 1 more level */
	}
	
	else if (comp > 0)
	{
		if (root->right == NULL)
			return NULL;
		
		return search(root->right, str); /* use recursion to decend 1 more level */
	}
	
	return root;
}


/* search if there is at least one entry label. yes = 1, no = 0 */
int searchEnt(binTree *root)
{
	if (root == NULL)
		return 0;
	
	if (root->isEntry)
		return 1;
	
	return searchEnt(root->left) || searchEnt(root->right);
}

/* search if there is at least one extern label. yes = 1, no = 0 */
int searchExt(binTree *root)
{
	if (root == NULL)
		return 0;
	
	if (root->isExternal)
		return 1;
	
	return searchExt(root->left) || searchExt(root->right);
}

/* free all allocated memory for this tree */
int freeTree(binTree **root)
{
	if (*root == NULL)
		return 0;
	
	freeTree(&((*root)->left));
	freeTree(&((*root)->right));
	
	free((*root)->str);
	free(*root);
	*root = NULL;
	return 0;
}


/* free binTree array */
int freeLabelTable(binTree ***arr, int fileNum)
{
	int i;
	binTree *root;
	
	if (arr == NULL || *arr == NULL)
		return 0;
	
	for (i = 0; i < fileNum; ++i)
	{
		root = (*arr)[i];
		if (root == NULL)
			return 0;
	
		freeTree(&root);
	}
	
	free(*arr);
	*arr = NULL;
	return 0;
}

int addIC(binTree **root, int IC)
{
	if (*root == NULL)
		return 0;
	
	if ((*root)->symbolType == DATA)
		(*root)->address += IC;
	
	addIC(&((*root)->left), IC);
	addIC(&((*root)->right), IC);
	return 0;
}


/* update this label (only on the labelTable of this file)- it's now an entry */
int addEntryLocal(char *str)
{
	binTree *node = search(labelTable[fileCounter], str);
	
	if (check_entryWithLocalDefinition(node, str) == ERROR)
		return ERROR;
	
	node->isEntry = 1;
	return 0;
}


/* update this label in all labelTables except of the file the entry is originaly from- these files can now extern it into themselves */
int addExternAcross(char *str, int type, int fileNum)
{
	int i;
	binTree *node;

	for (i = 0; i < fileNum; ++i)
	{
		if (i == fileCounter)	/* skip the file the entry is originaly from */
			continue;
		
		node = search(labelTable[i], str);
		
		if (node != NULL)
			node->isExternal = 1;
		else
			addNode(&labelTable[i], str, 0, type, 1, 0); /* address will be filled later in second pass */
	}
	
	return 0;
}




/* ============================================================================================ */



int addLineNode(lineNode **head, char *line, int address, int lineNum)
{
	lineNode *newNode = malloc(sizeof(lineNode)), *current;

	if (check_allocation(newNode) == ERROR)
		return ERROR;

	newNode->line = malloc(strlen(line) + 1);

	if (check_allocation(newNode->line) == ERROR)
		return ERROR;

	strcpy(newNode->line, line);
	newNode->address = address;
	newNode->lineNum = lineNum;
	newNode->next = NULL;

	if (*head == NULL)
	{
		*head = newNode;
		return 0;
	}

	current = *head;
	while (current->next != NULL)
		current = current->next;

	current->next = newNode;
	
	return 0;
}


int addICList(lineNode *dataList, int IC_FINAL) {
	lineNode *current;
	if (dataList==NULL)
		return 0;
	
	current = dataList;
	while (current != NULL) {
		current->address += IC_FINAL;
		current = current->next;
	}
	return 0;
}


																																		/* TEMP */
int printList(lineNode *node)
{
	if (node == NULL)
		return 0;
	
	printf("address: %d\t\tline: %s\n", node->address, node->line);
	printList(node->next);
	
	return 0;
}


/* free all allocated memory for this list */
int freeList(lineNode **node)
{
	if (*node == NULL)
		return 0;
	
	freeList(&((*node)->next));
	free(*node);
	return 0;
}


/* free all allocated memory for the array of lists. arr = pointer to pointer to an array of list pointers */
int freeListArr(lineNode ***arr, int fileNum)
{
	int i;
	
	if ((*arr) == NULL)
		return 0;
	
	for (i = 0; i < fileNum; ++i)
		if ((*arr)[i] != NULL)
			freeList(&((*arr)[i]));
	
	free(*arr);
	*arr = NULL;
	return 0;
}


/* check if the label is already in entryLineArr (if it was declared as an entry in this file) */
int isAlreadyEntry(char *label)
{
	lineNode *node;
	for (node = entryLineArr[fileCounter]; node != NULL; node = node->next)
		if (strcmp(node->line, label) == 0)
			return 1;
	
	return 0;
}

/* check if the label is already in externLineArr (if it was declared as extern in this file) */
int isAlreadyExtern(char *label)
{
	lineNode *node;
	for (node = externLineArr[fileCounter]; node != NULL; node = node->next)
		if (strcmp(node->line, label) == 0)
			return 1;
	
	return 0;
}



/* ================================================================================================================================ */



int freeNameArr(char ***arr, int fileNum) /* arr = pointer -> pointer -> array of char pointers */
{
	int i;
	
	if ((*arr) == NULL)
		return 0;
	
	for (i = 0; i < fileNum; ++i)
		if ((*arr)[i] != NULL)
			free((*arr)[i]); 	/* free each string */
	
	free(*arr);
	*arr = NULL;
	return 0;
}



/* ================================================================================================================================ */



int freeFileArr(FILE ***arr) /* arr = pointer to a pointer to an array of FILE pointers */
{
	if ((*arr) == NULL)
		return 0;
	
	free(*arr);
	*arr = NULL;
	return 0;
}



/* ================================================================================================================================ */



int initializeMacroArr()
{
	/* create an dinamic array of macros */
	macroArr = (macro **)calloc(1, sizeof(macro *));
	if (check_allocation(macroArr) == ERROR)
		return ERROR;
	
	return 0;
}


int addMacro(char *name)
{
	int len;
	macro **tempArr, *newMacro;
	
	len = (int)strlen(name);
	
	if (macroCounter != 0) /* need to reallocate memory */
	{
		tempArr = (macro **)realloc(macroArr, (macroCounter + 1) * sizeof(macro *));
		if (check_allocation(tempArr) == ERROR)
			return ERROR;
		macroArr = tempArr;
	}
	
	/* create macro */
	newMacro = (macro *)malloc(sizeof(macro));
	if (check_allocation(newMacro) == ERROR)
		return ERROR;
	
	/* allocate for lines */
	newMacro->macroLines = calloc(1, sizeof(char *));
	if (check_allocation(newMacro->macroLines) == ERROR)
		return ERROR;
	
	/* allocate for name */
	newMacro->name = (char *)malloc(len+1); /* +1 for \0 */
	if (check_allocation(newMacro->name) == ERROR)
		return ERROR;
	strcpy(newMacro->name, name);
	
	
	newMacro->lineAmount = 0;
	macroArr[macroCounter] = newMacro;
	macroCounter++;
	
	return 0;
}


int addLineToMacro(char *name, char *line)
{
	int i, lineAmount;
	char **tempLines, *lineCopy;
	
	for (i = 0; i < macroCounter && strcmp(name, macroArr[i]->name) != 0; ++i);
	
	if (i == macroCounter) /* macro doesn't exist */
		return ERROR;
	
	lineAmount = macroArr[i]->lineAmount;
	
	
	if (lineAmount != 0) /* need to reallocate memory */
	{
		tempLines = (char **)realloc(macroArr[i]->macroLines, sizeof(char *) * (lineAmount+1));
		if (check_allocation(tempLines) == ERROR)
			return ERROR;
		macroArr[i]->macroLines = tempLines;
	}
	
	/* allocate memory for the line */
	lineCopy = (char *)malloc(strlen(line) + 1); /* +1 for \0 */
	if (check_allocation(lineCopy) == ERROR)
		return ERROR;
	strcpy(lineCopy, line);
	
	macroArr[i]->macroLines[lineAmount] = lineCopy;
	macroArr[i]->lineAmount = lineAmount + 1;
	
	return 0;
}



/* free allocated memory for macroArr and all inner components */
int freeMacroArr()
{
	int i, j;
	macro *m;
	
	if (macroArr == NULL) /* macroArr not initialized yet */
		return 0;
	
	for (i = 0; i < macroCounter; ++i) /* for every macro */
	{
		m = macroArr[i];
		if (m == NULL)
			continue;
	
		/* free each line */
		if (m->macroLines != NULL)
		{
			for (j = 0; j < m->lineAmount; ++j)
			{
				if (m->macroLines[j] != NULL)
				{
					free(m->macroLines[j]);
					m->macroLines[j] = NULL;
				}
			}
			free(m->macroLines);
			m->macroLines = NULL;
		}
	
		/* free the macro name */
		if (m->name != NULL)
		{
			free(m->name);
			m->name = NULL;
		}
	
		/* free the macro object */
		free(m);
		macroArr[i] = NULL;
	}
	
	/* free the pointer array */
	free(macroArr);
	macroArr = NULL;
	macroCounter = 0; /* reset for reuse */

	return 0;
}

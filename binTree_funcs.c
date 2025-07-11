#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func_prototypes.h"



/* creates a node and fills it with the data entered (branches are set to NULL) */
binTree * makeNode(char *str, int address)
{
	binTree *node = malloc(sizeof(binTree));
	if (node == NULL)
		return NULL;
	
	node->str = str;
	node->address = address;
	node->left = NULL;
	node->right = NULL;
	
	return node;
}

/* allows to change the pointer to the left */
int setL(binTree *node, binTree *left)
{
	node->left = left;
	
	return 0;
}

/* allows to change the pointer to the right */
int setR(binTree *node, binTree *right)
{
	node->right = right;
	
	return 0;
}

/* prints the tree (in order) */
int printTree(binTree *root)																								/* TEMP */
{
	if (root == NULL)
		return 0;
	
	printTree(root->left);
	printf("str: %s\t\t\taddress: %d\n", root->str, root->address);
	printTree(root->right);
	
	return 0;
}


/* adds a node to the correct place in the tree. time compexity = O(log n) because every level cuts down the number of option by half. space complexity = O(n)- there is no waste of space because nodes are initiallized as needed and not ahead of time. Each node is O(n) because of the string but that is out of out control. In this prohect we will look at a node as a single unit so the space complexity of each node is O(1) 
The correct place in the tree is determined with the help of strcmp (from <string.h>) that compares 2 string an returns if they are identical or if one has a bigger value (lexicographically) */
int addNode(binTree *root, char *str, int address)
{
	int comp;
	
	if (root == NULL)
		return 0;
	
	comp = strcmp(str, root->str);
	
	
	if (comp < 0)
	{
		if (root->left == NULL)
		{
			setL(root, makeNode(str, address));
			return 0;
		}
		
		addNode(root->left, str, address); /* use recursion to decend 1 more level */
	}
	
	else if (comp > 0)
	{
		if (root->right == NULL)
		{
			setR(root, makeNode(str, address));
			return 0;
		}
		
		addNode(root->right, str, address); /* use recursion to decend 1 more level */
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

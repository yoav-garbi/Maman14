#include <stdio.h>
#include <stdlib.h>
#include "func_prototypes.h"




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


int setL(binTree *node, binTree *left)
{
	node->left = left;
	
	return 0;
}

int setR(binTree *node, binTree *right)
{
	node->right = right;
	
	return 0;
}


int printTree(binTree *root)																								/* TEMP */
{
	if (root == NULL)
		return 0;
	
	printTree(root->left);
	printf("str: %s\t\t\taddress: %d\n", root->str, root->address);
	printTree(root->right);
	
	return 0;
}

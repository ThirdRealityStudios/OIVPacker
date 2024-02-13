#ifndef PROJECT_OIV_TYPES_H
#define PROJECT_OIV_TYPES_H

#include <sys/stat.h>

typedef struct node Node;
typedef struct list List;
typedef struct tag Tag;

struct list
{
	Node *head, *tail;
	int size;
};

struct node
{
	Node *before;
	Node *next;
	char *value;
};

struct tag
{
	char *name;
	
	Node *attributeNames;
	Node *attributeValues;
	
	char *content;
};

#endif
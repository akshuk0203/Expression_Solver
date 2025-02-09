#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include<stdbool.h>


typedef enum{
	NUMERIC_OP, BOOLEAN_OP, PREDICATE_OP, VAR, BOOL_OPERAND, NUM_OPERAND
}nodeType;

typedef enum{
	BOOLEAN_VAR, NUMERIC_VAR
}varType;

typedef struct Node{
	nodeType type;
	struct Node *left;
	struct Node *right;
    union{
		char *_operator;
        float num_val;
        bool bool_val;
    }data;
	void *(*evaluate)(struct Node*);
}Node;

typedef struct{ 
	union{
		float num_value;
		bool bool_value;
	}value;
    varType type;
}ResolvedValue;

typedef struct Variable{
	bool isEvaluated;
    char *name;
    Node *expression;
	ResolvedValue value;
}Variable;

extern Variable variables[];
extern int variableCount;

Node* createNode(char *val, nodeType type, Node *left, Node *right);
Variable* findVariable(char *name);
void addVariable(char *name, Node *expression);
void extractDependencies(Node* root, int varIndex);
void addDependency(int from, int to);
void* evaluate_num_operand(Node *node);
void* evaluate_bool_operand(Node *node);
void* evaluate_numeric_operator(Node *node);
void* evaluate_boolean_operator(Node *node);
void* evaluate_predicate_operator(Node *node);
void *evaluate_var(Node *node);
ResolvedValue resolveVariable(char *name);
void topologicalSort();
void freeNode(Node *node);
void freeVariables();

#endif

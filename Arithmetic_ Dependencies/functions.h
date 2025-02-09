#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>

typedef struct Node{
    char *value;
    char *type;
    struct Node *left;
    struct Node *right;
}Node;

typedef struct Variable{
    char *name;
    Node *expression;
    bool evaluated;
    float value;
    int dependencies[10];
    int dependency_count;
}Variable;

extern Variable variables[];
extern int variableCount;

Node* createNode(char *val, char *type, Node *left, Node *right);
void addVariable(char *name, Node *expression);
float evaluate(Node* root);
void extractDependencies(Node* root, int varIndex);
void topologicalSort();

#endif

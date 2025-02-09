#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "functions.h"

#define MAX_VARIABLES 10

Variable variables[MAX_VARIABLES];
int variableCount = 0;

int graph[MAX_VARIABLES][MAX_VARIABLES] = {0};
int in_degree[MAX_VARIABLES] = {0};

Node* createNode(char *val, char *type, Node *left, Node *right) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->value = strdup(val);
    node->type = strdup(type);
    node->left = left;
    node->right = right;
    return node;
}

Variable* findVariable(char *name){
    for (int i = 0; i < variableCount; i++){
        if (strcmp(variables[i].name, name) == 0)
            return &variables[i];
    }
    return NULL;
}

void addDependency(int from, int to){
    graph[from][to] = 1;
    in_degree[to]++;
}

void addVariable(char *name, Node *expression){
    if (findVariable(name)) {
        fprintf(stderr, "Error: Declaration conflicts for variable %s\n", name);
        exit(EXIT_FAILURE);
    }
    
    variables[variableCount].name = strdup(name);
    variables[variableCount].expression = expression;
    variables[variableCount].evaluated = false;
    variables[variableCount].dependency_count = 0;
    variableCount++;
}

void extractDependencies(Node* root, int varIndex){
    if (!root) return;
    if (strcmp(root->type, "operand") == 0 && isalpha(root->value[0])){
        Variable *depVar= findVariable(root->value);
        if (!depVar){
            fprintf(stderr, "Error: Undefined variable '%s'\n", root->value);
            exit(EXIT_FAILURE);
        }
        int depIndex= depVar-variables;
        addDependency(depIndex, varIndex);
    }
    extractDependencies(root->left, varIndex);
    extractDependencies(root->right, varIndex);
}

float evaluate(Node* root);

float resolveVariable(char *name){
    Variable *var = findVariable(name);
    if (!var){
        fprintf(stderr, "Error: Undefined variable '%s'\n", name);
        exit(EXIT_FAILURE);
    }
    if (!var->evaluated){
        var->value = evaluate(var->expression);
        var->evaluated = true;
    }
    return var->value;
}

float evaluate(Node* root){
    if (!root) return 0;
    
    if (strcmp(root->type, "operand") == 0){
        if (isalpha(root->value[0])) {
            return resolveVariable(root->value);
        } else {
            return atof(root->value);
        }
    }

    float left_val= evaluate(root->left);
    float right_val= evaluate(root->right);

    if (strcmp(root->value, "+") == 0) return left_val + right_val;
    if (strcmp(root->value, "-") == 0) return left_val - right_val;
    if (strcmp(root->value, "*") == 0) return left_val * right_val;
    if (strcmp(root->value, "/") == 0) {
        if (right_val == 0) {
            fprintf(stderr, "Error: Division by zero\n");
            exit(EXIT_FAILURE);
        }
        return left_val / right_val;
    }
    return 0;
}

void topologicalSort(){
    int queue[MAX_VARIABLES], front = 0, rear = 0;

    for(int i=0; i<variableCount; i++){
        if(in_degree[i] == 0){
            queue[rear++]= i;
        }
    }

    int processed = 0;
	while(front < rear){
        int current = queue[front++];
        Variable *var = &variables[current];

        var->value = evaluate(var->expression);
        var->evaluated = true;

        printf("%s = %.2f\n", var->name, var->value);

        for(int i = 0; i < variableCount; i++){
            if(graph[current][i]){
                in_degree[i]--;
                if (in_degree[i] == 0){
                    queue[rear++] = i;
                }
            }
        }
        processed++;
    }

    if (processed != variableCount){
        fprintf(stderr, "Error: Circular dependency detected\n");
        exit(EXIT_FAILURE);
    }
}

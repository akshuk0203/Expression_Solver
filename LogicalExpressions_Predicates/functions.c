#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "functions.h"

#define MAX_VARIABLES 20

Variable variables[MAX_VARIABLES];
int variableCount = 0;

int graph[MAX_VARIABLES][MAX_VARIABLES] = {0};
int in_degree[MAX_VARIABLES] = {0};

Node* createNode(char *val, nodeType type, Node *left, Node *right) {
    Node* node = (Node*) malloc(sizeof(Node));
	if(!node){
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    node->type = type;
    node->left = left;
    node->right = right;

	switch(type){
		case NUM_OPERAND:
			node->data.num_val = strtof(val, NULL);
			node->evaluate = evaluate_num_operand;
			break;

		case BOOL_OPERAND:
            node->data.bool_val = (strcmp(val, "T") == 0);
            node->evaluate = evaluate_bool_operand;
            break;

		case NUMERIC_OP:
            node->data._operator = strdup(val);
            node->evaluate = evaluate_numeric_operator;
            break;

        case BOOLEAN_OP:
            node->data._operator = strdup(val);
            node->evaluate = evaluate_boolean_operator;
            break;

        case PREDICATE_OP:
            node->data._operator = strdup(val);
            node->evaluate = evaluate_predicate_operator;
            break;

        case VAR:
            node->data._operator = strdup(val);
            node->evaluate = evaluate_var; 	//will be evaluated later
            break;

		default:
			fprintf(stderr,"Invalid node type\n");
            free(node);
            return NULL;
	}
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
	variables[variableCount].isEvaluated = false;
    variableCount++;
}

void extractDependencies(Node* root, int varIndex){
    if (!root) return;
    if (root->type == VAR){
        Variable *depVar= findVariable(root->data._operator);
        if (!depVar){
            fprintf(stderr, "Error: Undefined variable '%s'\n",root->data._operator);
            exit(EXIT_FAILURE);
        }
        int depIndex= depVar-variables;
        addDependency(depIndex, varIndex);
    }
    extractDependencies(root->left, varIndex);
    extractDependencies(root->right, varIndex);
}

float evaluate(Node* root);

ResolvedValue resolveVariable(char *name){
    Variable *var = findVariable(name);
    if (!var){
        fprintf(stderr, "Error: Undefined variable '%s'\n", name);
        exit(EXIT_FAILURE);
    }
    if (!var->isEvaluated){
		ResolvedValue tempResult;

        if (var->expression->type == NUMERIC_OP || var->expression->type == NUM_OPERAND){
            tempResult.value.num_value = *((float *)var->expression->evaluate(var->expression));
            tempResult.type = NUMERIC_VAR;
        } 
		else if (var->expression->type == BOOLEAN_OP || var->expression->type == BOOL_OPERAND || var->expression->type == PREDICATE_OP) {
            tempResult.value.bool_value = *((bool *)var->expression->evaluate(var->expression));
            tempResult.type = BOOLEAN_VAR;
        } 
		else {
            fprintf(stderr, "Error: Unsupported variable type\n");
            exit(EXIT_FAILURE);
        }
		var->value = tempResult;
    }
	var->isEvaluated = true;
	return var->value;
}

void *evaluate_num_operand(Node *node) {
    return &(node->data.num_val);
}

void *evaluate_bool_operand(Node *node) {
    return &(node->data.bool_val);
}

void *evaluate_numeric_operator(Node *node) {
    if (!node->left || !node->right || !node) {
        fprintf(stderr, "Error: Invalid numeric operator node\n");
        exit(EXIT_FAILURE);
    }

    float *left_val = (float *)node->left->evaluate(node->left);
    float *right_val = (float *)node->right->evaluate(node->right);

	if (!left_val || !right_val) {
        fprintf(stderr, "Error: Evaluation returned NULL in numeric operator\n");
        exit(EXIT_FAILURE);
    }

    float *result = malloc(sizeof(float));

    if (strcmp(node->data._operator, "+") == 0) *result = *left_val + *right_val;
    else if (strcmp(node->data._operator, "-") == 0) *result = *left_val - *right_val;
    else if (strcmp(node->data._operator, "*") == 0) *result = *left_val * *right_val;
    else if (strcmp(node->data._operator, "/") == 0) {
        if (*right_val == 0) {
            fprintf(stderr, "Error: Division by zero\n");
            exit(EXIT_FAILURE);
        }
        *result = *left_val / *right_val;
    } else {
        fprintf(stderr, "Error: Unknown numeric operator %s\n", node->data._operator);
        exit(EXIT_FAILURE);
    }

    return result;
}

void *evaluate_boolean_operator(Node *node) {
	bool *result = malloc(sizeof(bool));
	if (strcmp(node->data._operator, "!") == 0) {
        if (!node->right) {
            fprintf(stderr, "Error: Invalid boolean operator node for '!'\n");
            exit(EXIT_FAILURE);
        }
        bool *right_val = (bool *)node->right->evaluate(node->right);
        *result = !(*right_val);
    }
	else{
		if (!node->left || !node->right){
		    fprintf(stderr, "Error: Invalid boolean operator node\n");
		    exit(EXIT_FAILURE);
		}

		bool *left_val = (bool *)node->left->evaluate(node->left);
		bool *right_val = (bool *)node->right->evaluate(node->right);

		if(strcmp(node->data._operator, "&&") == 0) 
			*result = *left_val && *right_val;
		else if(strcmp(node->data._operator, "||") == 0) 
			*result = *left_val || *right_val;
		else if(strcmp(node->data._operator, "!") == 0)
		 	*result =  !(*right_val);
		else if(strcmp(node->data._operator, "==") == 0)
			*result = *left_val == *right_val;
		else if(strcmp(node->data._operator, "!=") == 0)
			*result = *left_val != *right_val;
		else {
		    fprintf(stderr, "Error: Unknown boolean operator %s\n", node->data._operator);
		    exit(EXIT_FAILURE);
		}
	}
    return result;
}

void *evaluate_predicate_operator(Node *node) {
    if (!node->left || !node->right) {
        fprintf(stderr, "Error: Invalid predicate operator node\n");
        exit(EXIT_FAILURE);
    }

    float *left_val = (float *)node->left->evaluate(node->left);
    float *right_val = (float *)node->right->evaluate(node->right);

    bool *result = malloc(sizeof(bool));

    if (strcmp(node->data._operator, "==") == 0) *result = (*left_val == *right_val);
    else if (strcmp(node->data._operator, "!=") == 0) *result = (*left_val != *right_val);
    else if (strcmp(node->data._operator, "<=") == 0) *result = (*left_val <= *right_val);
	else if (strcmp(node->data._operator, ">=") == 0) *result = (*left_val >= *right_val);
	else if (strcmp(node->data._operator, "<") == 0) *result = (*left_val < *right_val);
	else if (strcmp(node->data._operator, ">") == 0) *result = (*left_val > *right_val);
    else {
        fprintf(stderr, "Error: Unknown predicate operator %s\n", node->data._operator);
        exit(EXIT_FAILURE);
    }

    return result;
}

void *evaluate_var(Node *node) {
    Variable *var = findVariable(node->data._operator);
    if (!var) {
        fprintf(stderr, "Error: Undefined variable '%s'\n", node->data._operator);
        exit(EXIT_FAILURE);
    }
    if (!var->isEvaluated) {
        fprintf(stderr, "Error: Variable '%s' has not been evaluated yet\n", node->data._operator);
        exit(EXIT_FAILURE);
    }
    if (var->value.type == NUMERIC_VAR) {
        return &(var->value.value.num_value);
    } else if (var->value.type == BOOLEAN_VAR) {
        return &(var->value.value.bool_value);
    } else {
        fprintf(stderr, "Error: Unsupported variable type for '%s'\n", node->data._operator);
        exit(EXIT_FAILURE);
    }
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

        var->value = resolveVariable(var->name);

        if(var->value.type == NUMERIC_VAR){
            printf("%s = %.2f\n", var->name, var->value.value.num_value);
        } 
		else if (var->value.type == BOOLEAN_VAR){
            printf("%s = %s\n", var->name, var->value.value.bool_value ? "true" : "false");
        }

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

void freeNode(Node *node) {
    if (!node) return;
    if (node->type == NUMERIC_OP || node->type == BOOLEAN_OP || node->type == PREDICATE_OP || node->type == VAR) {
        free(node->data._operator);
    }
    freeNode(node->left);
    freeNode(node->right);
    free(node);
}

void freeVariables() {
    for (int i = 0; i < variableCount; i++) {
        free(variables[i].name);
        freeNode(variables[i].expression);
    }
}

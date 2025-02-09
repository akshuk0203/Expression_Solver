%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "parser.tab.h"

    typedef struct Node {
        char *value;
        char *type;
        struct Node *left;
        struct Node *right;
    } Node;

    Node* createNode(char *val, char *type, Node *left, Node *right) {
        Node* node = (Node*) malloc(sizeof(Node));
        node->value = strdup(val);
        node->type = strdup(type);
        node->left = left;
        node->right = right;
        return node;
    }

    void inorderTraversal(Node* root) {
        if (!root) return;
        inorderTraversal(root->left);
        printf("%s ", root->value);
        inorderTraversal(root->right);
    }
    
    void preorderTraversal(Node* root) {
        if (!root) return;
        printf("%s ", root->value);
        preorderTraversal(root->left);
        preorderTraversal(root->right);
    }

	float evaluate(Node* root){
		if(!root) return 0;
		if (strcmp(root->type, "operand") == 0)
    		return atof(root->value);
			
		float left_val= evaluate(root->left);
		float right_val= evaluate(root->right);
		
		if(strcmp(root->value,"+")==0)
			return left_val+ right_val;
		if (strcmp(root->value, "-") == 0)
            return left_val - right_val;
        if (strcmp(root->value, "*") == 0)
            return left_val * right_val;
        if (strcmp(root->value, "/") == 0)
            return left_val / right_val;
        if (strcmp(root->value, "()") == 0)
            return left_val;
            
        return 0;
	}
	
    extern int yylex();
    extern int yyparse();
    void yyerror(const char *s);

    Node* root;
%}

%union {
    char *str;
    struct Node *node;
}

%token <str> NUMBER
%token PLUS MINUS MUL DIV LPAREN RPAREN
%type <node> expr term factor

%%
input:
    | input expr '\n' {
        root = $2;
        printf("Preorder Traversal: ");
        preorderTraversal(root);
        printf("\nInorder Traversal: ");
        inorderTraversal(root);
        printf("\nResult=\t%.2f\n",evaluate(root));
    }
    ;

expr:
    term
    | expr PLUS term { $$ = createNode("+", "operator", $1, $3); }
    | expr MINUS term { $$ = createNode("-", "operator", $1, $3); }
    ;

term:
    factor
    | term MUL factor { $$ = createNode("*", "operator", $1, $3); }
    | term DIV factor { $$ = createNode("/", "operator", $1, $3); 
    					if ($3 == 0) {
							fprintf(stderr, "Error: Division by zero\n");
							exit(EXIT_FAILURE);
						}}
    ;

factor:
    NUMBER { $$ = createNode($1, "operand", NULL, NULL); }
    | LPAREN expr RPAREN { $$ = createNode("()", "operator", $2, NULL); }
    ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main() {
    printf("Enter an expression:\n");
    yyparse();
    return 0;
}

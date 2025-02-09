all: calculator run

calculator: lex.yy.c parser.tab.c
	gcc -o calculator lex.yy.c parser.tab.c -lm

parser.tab.c: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.c
	flex lexer.l

run: calculator
	./calculator

clean:
	rm -f parser calculator lex.yy.c parser.tab.c parser.tab.h


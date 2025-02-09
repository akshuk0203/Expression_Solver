all: run

run : lex.yy.c parser.tab.c functions.o
	gcc -o calculator lex.yy.c parser.tab.c functions.o -lm && ./calculator input.txt

parser.tab.c: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.c
	flex lexer.l

functions.o: functions.c functions.h
	gcc -c functions.c

clean:
	rm -f a.out calculator lex.yy.c parser.tab.c parser.tab.h functions.o

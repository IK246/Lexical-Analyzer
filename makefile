compile.out: main.o lexical.o tokens.o
	gcc main.c lexical.c tokens.c -o compile.out
main.o: main.c
	gcc -c main.c
lexical.o: lexical.c
	gcc -c lexical.c
tokens.o: tokens.c
	gcc -c tokens.c
clean:
	rm *.out *.o

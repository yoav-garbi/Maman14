assembler: assembler.o errors.o
	gcc -Wall -pedantic -ansi assembler.o errors.o -o assembler

assembler.o: assembler.c
	gcc -c -Wall -pedantic -ansi assembler.c -o assembler.o
	
errors.o: errors.c
	gcc -c -Wall -pedantic -ansi errors.c -o errors.o

assembler: assembler.o errors.o general_funcs.o binTree_funcs.o io.o
	gcc -Wall -pedantic -ansi assembler.o errors.o general_funcs.o binTree_funcs.o io.o -o assembler

assembler.o: assembler.c
	gcc -c -Wall -pedantic -ansi assembler.c -o assembler.o
	
errors.o: errors.c
	gcc -c -Wall -pedantic -ansi errors.c -o errors.o
	
general_funcs.o: general_funcs.c
	gcc -c -Wall -pedantic -ansi general_funcs.c -o general_funcs.o
	
binTree_funcs.o: binTree_funcs.c
	gcc -c -Wall -pedantic -ansi binTree_funcs.c -o binTree_funcs.o
	
io.o: io.c
	gcc -c -Wall -pedantic -ansi io.c -o io.o

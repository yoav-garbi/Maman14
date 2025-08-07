assembler: assembler.o errors.o general_funcs.o struct_funcs.o io.o firstPass.o secondPass.o
	gcc -Wall -pedantic -ansi assembler.o errors.o general_funcs.o struct_funcs.o io.o firstPass.o secondPass.o -o assembler

assembler.o: assembler.c
	gcc -c -Wall -pedantic -ansi assembler.c -o assembler.o
	
errors.o: errors.c
	gcc -c -Wall -pedantic -ansi errors.c -o errors.o
	
general_funcs.o: general_funcs.c
	gcc -c -Wall -pedantic -ansi general_funcs.c -o general_funcs.o
	
struct_funcs.o: struct_funcs.c
	gcc -c -Wall -pedantic -ansi struct_funcs.c -o struct_funcs.o
	
io.o: io.c
	gcc -c -Wall -pedantic -ansi io.c -o io.o

firstPass.o: firstPass.c
	gcc -c -Wall -pedantic -ansi firstPass.c -o firstPass.o

secondPass.o: secondPass.c
	gcc -c -Wall -pedantic -ansi secondPass.c -o secondPass.o

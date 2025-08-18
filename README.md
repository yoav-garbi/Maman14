This code acts as an assembler to our made up assembly language. To use it, run "./assembler" followed by the names of your .as source files. Such as:
./assembler file1.as file2.as ...

The output will be either:
1. A series of error messages recording all errors found in your code (in stdout). Files will stop being created if an error is detected
2. A series of files and extension files:
     a. (file).am - a copy of the .as file but all macros are opened in the code, and all notes and blank lines are removed
     b. (file).ob - the actual runfile, in our made up base 4. In the right there is the addresses of each line. In the left there is the actual code and data. The first line shows the IC and DC
     c. (file).ent - an extension file showing the addresses of all '.entry' directive lines
     d. (file).ext - an extension file showing the addresses of all lines using a label that has been declared as 'extern' in the matching source file

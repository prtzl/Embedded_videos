# Manual building C/C++ application

To build C/C++ files by hand you will need GCC(CC) and G++ compilers. 

To compile a source file:
```
gcc -c <file_name>.c
```

If the file relies on headers that are **NOT** in the same folder as the source file, you have to include it with the `-I` include path:
```
gcc -c -I<directory> <file_name>.c
```

To compile multiple source files, just list them in series:
```
gcc -c <list_of_source_files ...>
```

To link all object files into executable:
```
gcc -o <executable_name> <list_of_object_files ...>
```

## Example:
```
gcc -c main.c Test.c
gcc -o executable main.o Test.o
```

Note: 
No need for include `-I` argument as gcc tries to find them in the same folder as the source file.

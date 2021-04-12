all:UNIX_Shell.o UNIX_Shell_bug_fix.o
	gcc -o UNIX_Shell UNIX_Shell.o
	gcc -o UNIX_Shell_bug_fix UNIX_Shell_bug_fix.o

UNIX_Shell.o:UNIX_Shell.c
	gcc -c UNIX_Shell.c

UNIX_Shell_bug_fix.o: UNIX_Shell_bug_fix.c
	gcc -c UNIX_Shell_bug_fix.c

clean:
	rm *.o UNIX_Shell *.txt UNIX_Shell_bug_fix
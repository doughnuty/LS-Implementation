# -*- Makefile -*-

#target: dependencies
#	action

my_ls: my_ls.o scanDir.o myGetopt.o
	gcc my_ls.o scanDir.o myGetopt.o -o my_ls  -Wall -Wextra -Werror
my_ls.o: my_ls.c my_ls.h
	gcc -c my_ls.c my_ls.h   -Wall -Wextra -Werror
scanDir.o: scanDir.c my_ls.h
	gcc -c scanDir.c my_ls.h  -Wall -Wextra -Werror
myGetopt.o: myGetopt.c my_ls.h
	gcc -c myGetopt.c my_ls.h  -Wall -Wextra -Werror
clean:
	rm -f *.o *.gch
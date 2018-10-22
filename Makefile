bin: cache.c test.c
	gcc -o bin -O3 -Wall -Wextra -pedantic cache.c test.c
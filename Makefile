bin: cache.c test.c
	g++ -o bin -O3 -Wall -Wextra -pedantic cache.c TDDtest.c
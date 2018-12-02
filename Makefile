bin: cache.c test.c
	g++ -o bin -O3 -Wall -Wextra -pedantic cache.c TDDtest.c

networkTDDtest: testing_client.c networkTDDtest.c
	g++ -g -o networkTDDtest -O3 -Wall -Wextra -pedantic testing_client.c networkTDDtest.c

server: cache.c server.c
	gcc -g -o server -O3 -Wall -Wextra -pedantic cache.c server.c
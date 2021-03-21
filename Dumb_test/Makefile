CC = g++
CFLAGS = -Wall -Wextra
DEBUG = -g -O0

build: test

test: test_xboard_dumb.cpp
	$(CC) $(CFLAGS) $(DEBUG) $^ -o $@

run: test
	./test

clean:
	rm -f test *.o *.debug
CC = g++
CFLAGS = -Wall -Wextra
DEBUG = -g -O0

BINARY = duca

SOURCE_FILES = main.o logger.o xboardHandler.o engine.o board.o moveChecker.o moveGen.o utils.o

build: $(BINARY)

%.o: %.cpp
	$(CC) $(CFLAGS) $(DEBUG) -c $^ -o $@

$(BINARY): $(SOURCE_FILES)
	$(CC) $(CFLAGS) $(DEBUG) $^ -o $@

run: $(BINARY)
	./$(BINARY)

clean:
	rm -f $(BINARY) *.o *.debug

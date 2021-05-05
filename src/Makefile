# Copyright 2021 DucaPowr Team
CC = g++
CFLAGS = -Wall -Wextra -g -Wno-unknown-pragmas
DEBUG =

BINARY = duca

SOURCE_FILES = \
	main.cpp \
	logger.cpp \
	xboardHandler.cpp \
	engine.cpp \
	board.cpp \
	moveChecker.cpp \
	moveGen.cpp \
	utils.cpp \

OBJECT_FILES = $(SOURCE_FILES:.cpp=.o)

build: $(BINARY)

%.o: %.cpp %.h
	$(CC) $(CFLAGS) $(DEBUG) -c $< -o $@

$(BINARY): $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(DEBUG) $^ -o $@

run: $(BINARY)
	./$(BINARY)

clean:
	rm -f $(BINARY) *.o *.debug

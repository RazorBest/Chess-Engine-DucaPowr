# Copyright 2021 DucaPowr Team
CC = g++
CFLAGS = -Wall -Wextra -Wno-unknown-pragmas
DEBUG =

SRC = ../src

SOURCES_TEST = $(wildcard $(SRC)/*.cpp)

SOURCES_TEST := $(filter-out $(SRC)/main.cpp, $(SOURCES_TEST))

SOURCE_FILES = \
    test.cpp \
	testGenerator.cpp \
	$(SOURCES_TEST)
								                                                                                
OBJECT_FILES = $(SOURCE_FILES:.cpp=.o)

BINARY = test

build: $(BINARY)

%.o: %.cpp
	$(CC) $(CFLAGS) $(DEBUG) -c $^ -o $@

$(BINARY): $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(DEBUG) $^ -o $@

run: $(BINARY)
	./$(BINARY)

clean:
	rm -f $(BINARY) $(OBJECT_FILES) *.debug

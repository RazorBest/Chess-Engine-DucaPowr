CC = g++
CFLAGS = -Wall -Wextra
DEBUG = -g -O0

BINARY = duca

build: $(BINARY)

main.o: main.cpp
	$(CC) $(CFLAGS) $(DEBUG) -c $^ -o $@

logger.o: logger.cpp
	$(CC) $(CFLAGS) $(DEBUG) -c $^ -o $@

xboardHandler.o: xboardHandler.cpp
	$(CC) $(CFLAGS) $(DEBUG) -c $^ -o $@

engine.o: engine.cpp
	$(CC) $(CFLAGS) $(DEBUG) -c $^ -o $@

$(BINARY): main.o logger.o xboardHandler.o engine.o
	$(CC) $(CFLAGS) $(DEBUG) $^ -o $@

run: $(BINARY)
	./$(BINARY)

clean:
	rm -f $(BINARY) *.o *.debug

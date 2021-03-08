CC = g++
CFLAGS = -g

BINARY = duca

build: $(BINARY)

main.o: main.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(BINARY): main.o
	$(CC) $(CFLAGS) $^ -o $@

run: $(BINARY)
	./$(BINARY)

clean:
	rm -f $(BINARY) *.o

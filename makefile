CC=gcc
CFLAGS=-c -ggdb -Wall
LDFLAGS= -lSDL2
SOURCES=chip8.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=bin/chip8

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm bin/chip8
	rm chip8.o
	
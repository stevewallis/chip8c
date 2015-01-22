CC=cc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=chip8.c
OBJECTS=$(SOURCES:.cp=.o)
EXECUTABLE=bin/chip8

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@


CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -std=c99
LDFLAGS =-lGL -lglfw -lm -lGLEW -lktx
INCLUDES = -Iheaders/ -Ilibs/perlin-lib/
SOURCES = src/main.c
OUTPUT = bin/linux_x86-64/executable


all: $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT) $(SOURCES) $(LDFLAGS)
	./$(OUTPUT)
check:
	clang-tidy $(SOURCES)


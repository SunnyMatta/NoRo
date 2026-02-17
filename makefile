CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS =-Lsources/perlin-lib/ -lglobal -lGL -lglfw -lm -lGLEW
INCLUDES = -IADDITIONALS/ -Isources/perlin-lib/
SOURCES = CODEBASE/main.C CODEBASE/perlin.c
OUTPUT = BINARY/LINUX_x86-64/executable


all: $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT) $(SOURCES) $(LDFLAGS)
	./$(OUTPUT)
check:
	clang-tidy $(SOURCES)

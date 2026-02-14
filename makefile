CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS = -lGL -lglfw -lm -lGLEW
INCLUDES = -IADDITIONALS/
SOURCES = CODEBASE/main.C
OUTPUT = BINARY/LINUX_x86-64/executable


all: $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT) $(SOURCES) $(LDFLAGS)
	./$(OUTPUT)
check:
	clang-tidy $(SOURCES)
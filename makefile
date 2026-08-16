CC = gcc
CFLAGS = -Wall -ansi -pedantic -std=c89
TARGET = assembler

# Automatically find all .c files in the directory
SRCS = $(wildcard *.c)

# Convert all .c filenames to .o filenames
OBJS = $(patsubst %.c, %.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
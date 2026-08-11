CC = gcc
CFLAGS = -Wall -ansi -pedantic -std=c99
TARGET = assembler
OBJS = main.o consts.o macro_table.o pre_assembler.o reserved_word.o utils.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
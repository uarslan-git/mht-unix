CC = gcc
CFLAGS = -Wall -O2
TARGET = memhack
OBJS = main.o memhack.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c memhack.h
	$(CC) $(CFLAGS) -c main.c

memhack.o: memhack.c memhack.h
	$(CC) $(CFLAGS) -c memhack.c

clean:
	rm -f $(TARGET) $(OBJS)

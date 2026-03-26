CC = gcc
CFLAGS = -g

SRC = main.c src/*.c src/internal/*.c
OUT = main

all:
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)

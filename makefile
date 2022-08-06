CC = gcc
CFLAGS = -Wall -g -ansi

main: main.c
	$(CC) $(CFLAGS) ./$< -o $@
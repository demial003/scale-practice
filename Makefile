CC := gcc
CFLAGS := -fsanitize=address -lasound -Wall -g 
main: main.c
	$(CC) $(CFLAGS) main.c -o main

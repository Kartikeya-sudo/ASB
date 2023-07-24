CC = gcc-13
CFLAGS = -Wextra -Wall -Werror -pedantic

object = browse
source = main

all:
	$(CC) $(CFLAGS) $(source).c -o $(object)

test:
	./$(object) http://www.paulgraham.com/read.html

clean:
	rm $(object)

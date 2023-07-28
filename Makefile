CC = gcc
CFLAGS = -Wextra -Wall -Werror -pedantic -I/usr/local/include/ -L/usr/local/lib/ -ltls

object = browse
source = main

all:
	$(CC) $(CFLAGS) $(source).c -o $(object)

test:
	./$(object) https://example.org/

clean:
	rm $(object)

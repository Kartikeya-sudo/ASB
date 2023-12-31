CC = gcc
CFLAGS = -Wextra -Wall -Werror -pedantic -I/usr/local/include/ -L/usr/local/lib/ -ltls

object = browse
sources = main prot/https prot/http parser/html structs/url

all:
	$(CC) $(CFLAGS) $(foreach source, $(sources), $(source).c) -o $(object)

test:
	./$(object) https://example.org/

clean:
	rm $(object)

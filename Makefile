CC = gcc
CFLAGS = -Wextra -Wall -Werror -pedantic -g -I/usr/local/include/ -L/usr/local/lib/ -ltls -lssl -lcrypto

object = browse
sources = main prot/https prot/http parser/html structs/url

all:
	$(CC) $(foreach source, $(sources), $(source).c) -o $(object) $(CFLAGS) 

test:
	./$(object) https://example.org/

clean:
	rm $(object)

#include "prot/prot.h"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: browse url\n");
        return 1;
    }

    Url* url = parse_url(argv[1]);


    // Returns zero if both strings are identical
    if (strcmp(url->protocol, "http") == 0) {
        http(url);
    } else if (strcmp(url->protocol, "https") == 0) {
        https(url);
    } else {
        fprintf(stderr, "Unknown protocol specified\n");
    }
}

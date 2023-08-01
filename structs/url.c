#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    char* protocol;
    char* hostname;
    char* path;
} Url;


Url* parse_url(char* string) {
    Url* result = (Url*) malloc(sizeof (Url)); // Heap allocate Url
    char* p = string; // Pointer to the given string


    char* proto = (char*) malloc(sizeof(char) * strlen(string));
    int i = 0;
    // Checks for :// in the string,
    // which indicates the specification of a protocol
    if (strstr(string, "://") != NULL) {
        while (*p != ':') {
            proto[i] = *p;
            p++;
            i++;
        }
        proto[i] = '\0';
        result->protocol = proto;
        printf("protocol: %s\n", result->protocol);
        p += 3;
    } else {
        result->protocol = "http";
    }

    char* hostname = (char*) malloc(sizeof(char) * strlen(string));
    i = 0;

    // At this point the pointer to the string should be past the ://
    while (p != NULL) {
        if (*p == '/' || *p == '\0') break;
        hostname[i] = *p;
        p++;
        i++;
    }

    hostname[i] = '\0';

    result->hostname = hostname;

    printf("hostname: %s\n", result->hostname);

    char* rest = (char*) malloc(sizeof(char) * strlen(string));
    i = 0;
    if (*p == '/') {
        while (p != NULL && *p != '\0') {
            rest[i] = *p;
            p++;
            i++;
        }
    result->path = rest;

    } else {
        result->path = "/";
    }


    printf("path: %s\n", result->path);

    return result;
}

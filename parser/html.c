#include <stdbool.h>
#include <stdio.h>

void html_parser(char* response) {
    char* pointer = response;

    bool in_tag = false;
    while (*pointer != '<') pointer++;

    while (pointer != NULL && *pointer != '\0') {

        if (*pointer == '<') {
            in_tag = true;
        } else if (*pointer == '>') {
            in_tag = false;
        } else {
            if (in_tag == false) {
                printf("%c", *pointer);
            }
        }

        pointer++;
    }
}

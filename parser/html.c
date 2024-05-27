#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum tags {
  HTML, STRING, TITLE,
  H1, H2, H3, H4,
} Tag;

typedef struct content {
  char* string;
  Tag tag;
} Content;

typedef struct queue {
  Content* content;
  struct queue* next;
  struct queue* desc;
} Queue;

void html_parser(char* response) {
    char* pointer = response;

		//Queue* html = malloc(sizeof(struct queue));

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

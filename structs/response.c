#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* TODO: Add error checking for malloc calls and memory deallocs */

typedef struct {
    char* response; // The response string
    size_t used; // The memory used
    size_t size; // The total memory
} Response;

Response* initResponse(size_t initialSize) {
    Response* resp = (Response*) malloc(sizeof(Response));

    resp->response = (char*) malloc(initialSize * sizeof(char));
    resp->used = 0;
    resp->size = initialSize;

    return resp;
}

void insertString(Response* resp, char* newString) {
    // The total required size is greater than the memory available
    // The extra 1 is for \0 to signify a string
    if (resp->used + (strlen(newString)) + 1 >= resp->size) {
        resp->size += strlen(newString) + 1;
        resp->response = realloc(resp->response, resp->size * (sizeof (char)));
    }

    memcpy(resp->response + resp->used, newString, strlen(newString));
    resp->used += strlen(newString);

    return ;
}

void freeResponse(Response* resp) {
    free(resp->response);
    resp->response = NULL;
    resp->size = 0;
    resp->used = 0;

    free(resp);
    return ;
}

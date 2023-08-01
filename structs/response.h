#include <stdlib.h>

typedef struct {
    char* response; // The response string
    size_t used; // The memory used
    size_t size; // The total memory
} Response;

Response* initResponse(size_t initialSize);

void insertString(Response* resp, char* newString);

void freeResponse(Response* resp);

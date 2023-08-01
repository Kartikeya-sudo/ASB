typedef struct {
    char* protocol;
    char* hostname;
    char* path;
} Url;


Url* parse_url(char* string);

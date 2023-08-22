#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
/* Networking headers */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <tls.h>

#define MAXDATA 500

#include "../structs/url.h"

#include "../parser/html.h"


void http(Url* url);
void https(Url* url);

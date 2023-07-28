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

void http(Url* url) {
    int status, sockfd;
    struct addrinfo hints;
    struct addrinfo* servinfo, *p;

    char buf[MAXDATA];

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC; // Doesn't matter if its IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Require TCP stream socket
    hints.ai_flags = AI_PASSIVE; // Fills out the IP address of the current machine

    // Gets the addrinfo of the given website, returns zero if affirmative status
    if (strcmp(url->protocol, "http") == 0) {
        if ((status = getaddrinfo(url->hostname, "80", &hints, &servinfo)) != 0) {
            fprintf(stderr, "Couldn't get the addrinfo of the website: %s\n", url->hostname);
            return ;
        }
    } else {
        fprintf(stderr, "Error in reading protocol\n");
        return ;
    }

    if (servinfo == NULL) {
        fprintf(stderr, "Struct addrinfo couldn't get details of the website\n");
    }

    printf("Got addrinfo\n");


    // Binds to the first possible result -> IP
    for (p = servinfo; p != NULL; p = p->ai_next) {

        if((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        printf("Opened socket\n");


        if((connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        printf("Connected to server\n");

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to connect\n");
        return ;
    }



    // GET request requires the end to have two blank lines
    char msg[MAXDATA];
    sprintf(msg, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", url->path, url->hostname);
    printf("Request: %s", msg);
    int len = strlen(msg);

    int sbytes;
    // Sends the GET request to the server
    if ((sbytes = send(sockfd, msg, len, 0)) != len) {
        printf("Complete GET request was not sent\n");
    }

    printf("Sent GET request, with total bytes: %d\n", sbytes);

    freeaddrinfo(servinfo);

    int nbytes;

    printf("Waiting for response\n");

    while ((nbytes = recv(sockfd, buf, MAXDATA-1, 0)) > 1) {
        buf[nbytes] = '\0';
        printf("%s", buf);
        memset(buf, 0, sizeof buf);
    }


    printf("\n");

    // Closes the socket to prevent further sending or reading
    close(sockfd);

    return ;
}

void https(Url* url) {
    struct tls_config* config = tls_config_new(); // Initializes, allocates and returns a tls config

    const char* root_ca = tls_default_ca_cert_file();
    tls_config_set_ca_file(config, root_ca); // Sets the root certificate

    printf("Certificates: %s\n", root_ca);

    struct tls* conn = tls_client(); // Intializes, allocates and returns a pointer to a client context

    if (conn == NULL) {
        fprintf(stderr, "Could not create client\n");
        return ;
    }

    // Configures the client connection
    if(tls_configure(conn, config) != 0) {
        fprintf(stderr, "Error in configuration\n");
        return ;
    }

    // 443 is the default port for https
    if(tls_connect(conn, url->hostname, url->protocol) != 0) {
        fprintf(stderr, "Unable to connect to: %s\n", url->hostname);
    }

    printf("Connected via tls\n");

    // GET request requires the end to have two blank lines
    char msg[MAXDATA];
    sprintf(msg, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", url->path, url->hostname);
    printf("Request: %s", msg);
    int len = strlen(msg);

    // Sends the request
    if(tls_write(conn, msg, len) == -1) {
        printf("Error while writing via tls: %s", tls_error(conn));
        return ;
    }

    printf("Request sent\n");

    int nbytes = 0;
    char buf[MAXDATA];

    while (true) {
        nbytes = tls_read(conn, buf, MAXDATA - 1);

        if (nbytes == TLS_WANT_POLLIN || nbytes == TLS_WANT_POLLOUT) {
            continue;
        }

        if (nbytes == -1) {
            fprintf(stderr, "Error in reading: %s\n", tls_error(conn));
            return ;
        }

        if (nbytes == 0) break;

        buf[nbytes] = '\0';
        printf("%s", buf);
        memset(buf, 0, nbytes);
    }


    tls_close(conn);

    printf("Closed connection\n");

    tls_free(conn);

    tls_config_free(config);

    return ;
}

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
/* Networking headers */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAXDATA 500

typedef struct {
    char* protocol;
    char* hostname;
    char* path;
} Url;

Url*
parse_url(char* string)
{
    Url* result = (Url*) malloc(sizeof (Url)); // Heap allocate Url
    char* p = string; // Pointer to the given string


    char* proto = (char*) malloc(sizeof(char) * strlen(string));
    int i = 0;
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

int
main(int argc, char* argv[])
{
    int status, sockfd;
    struct addrinfo hints;
    struct addrinfo* servinfo, *p;

    char buf[MAXDATA];

    if (argc != 2) {
        fprintf(stderr, "Usage: browse url\n");
        return 1;
    }

    Url* url = parse_url(argv[1]);

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC; // Doesn't matter if its IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Require TCP stream socket
    hints.ai_flags = AI_PASSIVE; // Fills out the IP address of the current machine

    // Gets the addrinfo of the given website
    if ((status = getaddrinfo(url->hostname, "80", &hints, &servinfo)) != 0) {
        fprintf(stderr, "Couldn't get the addrinfo of the website: %s\n", argv[1]);
        return 1;
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
        return 1;
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

    freeaddrinfo(servinfo);;

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

    return 0;
}

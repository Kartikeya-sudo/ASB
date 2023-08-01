#include "prot.h"

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

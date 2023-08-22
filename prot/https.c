#include "prot.h"

void https(Url* url) {
    struct tls_config* config = tls_config_new(); // Initializes, allocates and returns a tls config

    const char* root_ca = tls_default_ca_cert_file(); // Returns the path to the default root cert
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

    //Response* response = initResponse();

    char* new_string = calloc(MAXDATA, sizeof(char));
    if (new_string == NULL) {
        fprintf(stderr, "Error allocating space for string\n");
        exit(1);
    }
    int available = MAXDATA;
    int curr_size = 0;
    while (true) {
        char read[MAXDATA];
        int nbytes = 0;

        nbytes = tls_read(conn, read, MAXDATA - 1);

        if (nbytes == TLS_WANT_POLLIN || nbytes == TLS_WANT_POLLOUT) {
            continue;
        }

        if (nbytes == -1) {
            fprintf(stderr, "Error in reading: %s\n", tls_error(conn));
            return ;
        }

        if (nbytes == 0) break;

        read[nbytes] = '\0'; // Marks as a string

        if (available < nbytes) {
            new_string = realloc(new_string, curr_size + available + (nbytes - available + 1));
            if (new_string == NULL) {
                fprintf(stderr, "Error reallocating space for string\n");
                exit(1);
            }
            available += (nbytes - available + 1);
        }

        strncat(new_string, read, nbytes);
        curr_size += nbytes;
        available -= nbytes;

    }
    html_parser(new_string);

    new_string[curr_size + 1] = '\0';

    free(new_string);

    tls_close(conn);

    printf("Closed connection\n");

    tls_free(conn);

    tls_config_free(config);


    return ;
}

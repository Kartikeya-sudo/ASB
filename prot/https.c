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

    Response* response = initResponse(MAXDATA);

    while (true) {
        char buf[MAXDATA];
        int nbytes = 0;

        nbytes = tls_read(conn, buf, MAXDATA - 1);

        if (nbytes == TLS_WANT_POLLIN || nbytes == TLS_WANT_POLLOUT) {
            continue;
        }

        if (nbytes == -1) {
            fprintf(stderr, "Error in reading: %s\n", tls_error(conn));
            return ;
        }

        if (nbytes == 0) break;

        buf[nbytes] = '\0'; // Marks as a string
        insertString(response, buf);
    }
    //printf("%s", response->response);
    html_parser(response->response);

    freeResponse(response);


    tls_close(conn);

    printf("Closed connection\n");

    tls_free(conn);

    tls_config_free(config);

    return ;
}

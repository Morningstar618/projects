/* httpd */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LISTENADDR "127.0.0.1"

/* global */
char *error;

/* returns 0 on error, or it returns a socket file descriptor*/
int srv_init(int port)
{
    int s;
    struct sockaddr_in srv;

    s = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM = constant for creating TCP type sockets
    if (s < 0)
    {
        error = "socket() error";
        return 0;
    }

    srv.sin_family = AF_INET;                    // AF_INET = constant for IPv4
    srv.sin_port = htons(port);                  // Convert int port to network byte order
    srv.sin_addr.s_addr = inet_addr(LISTENADDR); // Convert string LISTENADDR to binary form

    if (bind(s, (struct sockaddr *)&srv, sizeof(srv)))
    {
        close(s);
        error = "bind() error";
        return 0;
    }

    if (listen(s, 5))
    {
        close(s);
        error = "listen() error";
        return 0;
    }

    return s;
}

/* returns 0 on error, or returns the new client's socket file descriptor */
int client_accept(int s)
{
    int c;
    socklen_t addrlen;
    struct sockaddr_in client;

    addrlen = 0;
    memset(&client, 0, sizeof(client)); // Initialize memory of client socket structure with 0 to remove garbage values
    c = accept(s, (struct sockaddr *)&client, &addrlen);
    if (c < 0)
    {
        error = "accept() error";
        return 0;
    }

    return c;
}

void client_conn(int s, int c)
{
    return;
}

int main(int argc, char *argv[])
{
    int s, c;
    char *port;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <listening_port>\n", argv[0]);
        return -1;
    }
    else
        port = argv[1];

    s = srv_init(atoi(port));
    if (!s)
    {
        fprintf(stderr, "%s\n", error);
        return -1;
    }

    printf("Listening on %s:%s\n", LISTENADDR, port);
    while (1)
    {
        c = client_accept(s);
        if (!c)
        {
            fprintf(stderr, "%s\n", error);
            continue;
        }

        printf("Incoming connection\n");
        if (!fork())
            client_conn(s, c);
    }

    return -1;
}

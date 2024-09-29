/* httpd */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LISTENADDR "127.0.0.1"

/* returns 0 on error, or it returns a socket file descriptor*/
int srv_init(int port);

int main(int argc, char *argv[])
{
    int s;
    char *port;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <listening_port>\n", argv[0]);
        return -1;
    }
    else
        port = argv[1];

    s = srv_init(atoi(port));
}

int srv_init(int port)
{
    int s;
    struct sockaddr_in srv;

    s = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM = constant for creating TCP type sockets

    if (s < 0)
        return 0;

    srv.sin_family = AF_INET;                    // AF_INET = constant for IPv4
    srv.sin_port = htons(port);                  // Convert int port to network byte order
    srv.sin_addr.s_addr = inet_addr(LISTENADDR); // Convert string LISTENADDR to binary form
}
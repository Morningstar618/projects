#include "toralize.h"

Req *request(const char *dstip, const int dstport)
{
    Req *req = malloc(REQ_SIZE);

    req->vn = 4;
    req->cd = 1;
    req->dstport = htons(dstport);
    req->dstip = inet_addr(dstip);
    strncpy(req->userid, USER_ID, 8);

    return req;
}

int main(int argc, char *argv[])
{
    char *host;
    int port, s;
    struct sockaddr_in sock;
    Req *req;
    Res *res;
    char buf[RES_SIZE];
    int success;
    char tmp[512];

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        return -1;
    }

    host = argv[1];
    port = atoi(argv[2]);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        perror("socket\n");
        return -1;
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXY_PORT);
    sock.sin_addr.s_addr = inet_addr(PROXY);

    // Connect to the Tor proxy network
    if (connect(s, (struct sockaddr *)&sock, sizeof(sock)))
    {
        perror("connect\n");
        return -1;
    }

    req = request(host, port);
    write(s, req, REQ_SIZE); // Send the request by writing the req data to the FD

    memset(buf, 0, RES_SIZE); // make every value in buf 0

    // Reply packet is received from the proxy server only when connection to host is established
    if (read(s, buf, RES_SIZE) < 1)
    {
        perror("read");
        free(req);
        close(s);

        return -1;
    }

    res = (Res *)buf;          // Assign char buffer that has res data to our res struct to deserialize packet
    success = (res->cd == 90); // Check is connection to host:port was successfull

    if (!success)
    {
        fprintf(stderr, "Unable to traverse the proxy, "
                        "error code: %d\n",
                res->cd);

        close(s);
        free(req);

        return -1;
    }

    printf("Successfully connected through the proxy to "
           "%s:%d\n",
           host, port);

    memset(tmp, 0, 512);

    // writing data to be sent to the host to a buffer
    snprintf(tmp, 511,
             "HEAD / HTTP/1.0\r\n"
             "Host: toralize.com\r\n"
             "\r\n");
    write(s, tmp, strlen(tmp));

    // setting tmp to 0 again to be able to put res data in it
    memset(tmp, 0, 512);

    // putting res data in tmp and printing it
    read(s, tmp, 511);
    printf("%s", tmp);

    close(s);
    free(req);

    return 0;
}
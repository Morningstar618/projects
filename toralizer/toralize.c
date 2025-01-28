#include "toralize.h"

Req *request(struct sockaddr_in *sock2)
{
    Req *req = malloc(REQ_SIZE);

    req->vn = 4;
    req->cd = 1;
    req->dstport = sock2->sin_port;
    req->dstip = sock2->sin_addr.s_addr;
    strncpy(req->userid, USER_ID, 8);

    return req;
}

int connect(int s2, const struct sockaddr *sock2, socklen_t addrlen)
{
    int s;
    struct sockaddr_in sock;
    Req *req;
    Res *res;
    char buf[RES_SIZE];
    int success;
    char tmp[512];

    int (*p)(int, const struct sockaddr *, socklen_t);

    p = dlsym(RTLD_NEXT, "connect");
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        perror("socket\n");
        return -1;
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXY_PORT);
    sock.sin_addr.s_addr = inet_addr(PROXY);

    if (p(s, (struct sockaddr *)&sock, sizeof(sock)))
    {
        perror("connect\n");
        return -1;
    }

    req = request((struct sockaddr_in *)sock2);
    write(s, req, REQ_SIZE);

    memset(buf, 0, RES_SIZE);

    if (read(s, buf, RES_SIZE) < 1)
    {
        perror("read");
        free(req);
        close(s);

        return -1;
    }

    res = (Res *)buf;
    success = (res->cd == 90);

    if (!success)
    {
        fprintf(stderr, "Unable to traverse the proxy, "
                        "error code: %d\n",
                res->cd);

        close(s);
        free(req);

        return -1;
    }

    printf("Connected through the proxy\n");

    dup2(s, s2);
    free(req);

    return 0;
}
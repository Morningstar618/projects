#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <dlfcn.h>

#define PROXY "127.0.0.1"
#define PROXY_PORT 9050
#define USER_ID "toraliz"
#define REQ_SIZE sizeof(struct proxy_request)
#define RES_SIZE sizeof(struct proxy_response)

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;

// SOCKS v4
struct proxy_request
{
    int8 vn;
    int8 cd;
    int16 dstport;
    int32 dstip;
    unsigned char userid[8];
};
typedef struct proxy_request Req;

struct proxy_response
{
    int8 vn;
    int8 cd;
    int16 _;
    int32 __;
};
typedef struct proxy_response Res;

Req *request(struct sockaddr_in *);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

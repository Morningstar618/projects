#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     //close
#include <sys/socket.h> //connect, socket
#include <arpa/inet.h>  //htons
#include <netinet/in.h> //inet_addr

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

Req *request(const char *, const int);
int main(int, char **);

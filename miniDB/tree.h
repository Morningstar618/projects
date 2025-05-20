#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <errno.h>

#define _GNU_SOURCE
#define TAGROOT     1 /* 00 01 */
#define TAGNODE     2 /* 00 10 */
#define TAGLEAF     4 /* 01 00 */

#define NOERROR     0

typedef void *Nullptr;
Nullptr nullptr = 0;

#define reterr(x)       \
    do {                \
        errno = (x);    \
        return nullptr; \
    } while (0)

//---------------------------------------------------------------------

typedef unsigned int uint32;
typedef unsigned short int uint16;
typedef unsigned char uint8;
typedef unsigned char Tag;

__attribute__((__packed__)) struct s_node {
    Tag tag;
    struct s_node *north;
    struct s_node *west;
    struct s_leaf *east;
    uint8 path[256]; 
};
typedef struct s_node Node;

__attribute__((__packed__)) struct s_leaf {
    Tag tag;
    union u_tree *west;
    struct s_leaf *east;
    uint8 key[128];
    uint8 *value;
    uint16 size;
};
typedef struct s_leaf Leaf;

union u_tree {
    Node n;
    Leaf l;
};
typedef union u_tree Tree;
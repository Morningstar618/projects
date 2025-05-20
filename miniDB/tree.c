#include "tree.h"

Tree root = {
    .n = {
        .tag = (TAGROOT | TAGNODE),
        .north = (Node *)&root,
        .west = NULL,
        .east = NULL,
        .path = "/"
    }
};

Node *createNode(Node *parent, uint8 *path) {
    assert(parent);

    errno = NOERROR;

    Node *n = (Node *)calloc(1, sizeof(Node));
    assert(n);

    parent->west = n;

    n->tag = TAGNODE;
    n->north = parent;
    strncpy((char *)n->path, (char *)path, 255);

    return n;
}

Leaf *findLastLeaf(Node *parentNode) {
    assert(parentNode);

    errno = NOERROR;

    if (!parentNode->east)
        reterr(NOERROR);

    Leaf *tail = parentNode->east;
    
    while (tail->east) {
        tail = tail->east;
    }

    return tail;

}

Leaf *createLeaf(Node *parent, uint8 *key, uint8 *value, int valueSize) {
    assert(parent);

    Leaf *leaf, *newLeaf;

    newLeaf = (Leaf *)calloc(1, sizeof(Leaf));
    assert(newLeaf);

    leaf = findLastLeaf(parent);

    if (!leaf)
        parent->east = newLeaf;
    else
        leaf->east = newLeaf;

    newLeaf->tag = TAGLEAF;
    newLeaf->west = (!leaf) ? (Tree *)parent : (Tree *) leaf;
    newLeaf->east = NULL;

    strncpy((char *)newLeaf->key, (char *)key, 127);

    newLeaf->value = (uint8 *)calloc(1, sizeof(valueSize));
    assert(newLeaf->value);
    strncpy((char *)newLeaf->value, (char *)value, valueSize);

    newLeaf->size = valueSize;

    return newLeaf;
}

int main() {
    Node *n, *n2;
    Leaf *l1, *l2;

    n = createNode((Node *)&root, (uint8 *)"/Users");
    assert(n);
    l1 = createLeaf(n, (uint8 *)"ayush", (uint8 *)"sre", 3);
    assert(l1);

    n2 = createNode(n, (uint8 *)"/Users/login");
    assert(n2);
    l2 = createLeaf(n2, (uint8 *)"ayush", (uint8 *)"password", 8);
    assert(l2);

    Leaf *l3 = createLeaf(n2, (uint8 *)"user1", (uint8 *)"pass1", 5);

    printf("root: %p\n", &root);
    printf("Node 1: %p\n", &n);
    printf("Node 2: %p\n", &n2);
    printf("Leaf 1: %p\n", &l1);
    printf("Leaf 2: %p\n", &l2);
    printf("Leaf 3: %p\n", &l3);


    free(n);
    free(n2);
    free(l1);
    free(l2);
}
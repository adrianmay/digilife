
#include <stdint.h>
#include <stddef.h>

typedef struct Node {
    size_t   count;    /* number of leaves */
    uint64_t weight;   /* subtree weight */

    union {
        struct {
            struct Node *left;
            struct Node *right;
        } internal;

        struct {
            void *item;
        } leaf;
    } u;
} Node;

#include <stdlib.h>

static Node *make_leaf(void *item, uint64_t weight) {
    Node *n = malloc(sizeof(Node));
    n->count  = 1;
    n->weight = weight;
    n->u.leaf.item = item;
    return n;
}

static Node *make_internal(Node *l, Node *r) {
    Node *n = malloc(sizeof(Node));
    n->count  = l->count + r->count;
    n->weight = l->weight + r->weight;
    n->u.internal.left  = l;
    n->u.internal.right = r;
    return n;
}

Node *insert(Node *root, void *item, uint64_t weight) {
    if (!root)
        return make_leaf(item, weight);

    if (root->count == 1)
        return make_internal(root, make_leaf(item, weight));

    Node *l = root->u.internal.left;
    Node *r = root->u.internal.right;

    if (l->count <= r->count)
        root->u.internal.left = insert(l, item, weight);
    else
        root->u.internal.right = insert(r, item, weight);

    root->count  = root->u.internal.left->count
                 + root->u.internal.right->count;
    root->weight = root->u.internal.left->weight
                 + root->u.internal.right->weight;
    return root;
}

static uint64_t rand_u64(uint64_t limit) {
    uint64_t r;
    uint64_t max = UINT64_MAX - (UINT64_MAX % limit);

    do {
        r = ((uint64_t)rand() << 32) ^ rand();
    } while (r >= max);

    return r % limit;
}

static void *draw_with_r(Node **nodep, uint64_t r) {
    Node *n = *nodep;

    /* leaf */
    if (n->count == 1) {
        void *item = n->u.leaf.item;
        free(n);
        *nodep = NULL;
        return item;
    }

    Node *l = n->u.internal.left;

    if (r < l->weight) {
        void *res = draw_with_r(&n->u.internal.left, r);

        if (!n->u.internal.left) {
            Node *tmp = n->u.internal.right;
            free(n);
            *nodep = tmp;
        } else {
            n->count  = n->u.internal.left->count
                      + n->u.internal.right->count;
            n->weight = n->u.internal.left->weight
                      + n->u.internal.right->weight;
        }
        return res;
    } else {
        r -= l->weight;
        void *res = draw_with_r(&n->u.internal.right, r);

        if (!n->u.internal.right) {
            Node *tmp = n->u.internal.left;
            free(n);
            *nodep = tmp;
        } else {
            n->count  = n->u.internal.left->count
                      + n->u.internal.right->count;
            n->weight = n->u.internal.left->weight
                      + n->u.internal.right->weight;
        }
        return res;
    }
}

void *draw(Node **rootp) {
    if (!*rootp) return NULL;
    return draw_with_r(rootp, rand_u64((*rootp)->weight));
}

void free_tree(Node *n) {
    if (!n) return;
    if (n->count != 1) {
        free_tree(n->u.internal.left);
        free_tree(n->u.internal.right);
    }
    free(n);
}


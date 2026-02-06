
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include "roulette.c"

/* Recursive verification of tree invariants */
static void verify_node(const Node *n, size_t *out_count, uint64_t *out_weight) {
    assert(n);

    if (n->count == 1) {
        /* leaf */
        assert(n->u.leaf.item != NULL);
        *out_count = 1;
        *out_weight = n->weight;
        return;
    }

    /* internal node */
    Node *l = n->u.internal.left;
    Node *r = n->u.internal.right;
    assert(l && r);

    size_t lc, rc;
    uint64_t lw, rw;

    verify_node(l, &lc, &lw);
    verify_node(r, &rc, &rw);

    assert(n->count == lc + rc);
    assert(n->weight == lw + rw);

    *out_count = n->count;
    *out_weight = n->weight;
}

void prop_structure_random_inserts(void) {
    Node *root = NULL;

    for (int i = 0; i < 1000; i++) {
        uint64_t w = (rand() % 100) + 1;
        int *payload = malloc(sizeof(int));
        *payload = i;
        root = insert(root, payload, w);

        size_t c;
        uint64_t tw;
        verify_node(root, &c, &tw);
        assert(c == (size_t)(i + 1));
    }

    free_tree(root);
}

void prop_draw_removes_one(void) {
    Node *root = NULL;
    int a=1, b=2, c=3;

    root = insert(root, &a, 1);
    root = insert(root, &b, 2);
    root = insert(root, &c, 3);

    size_t before_count = root->count;
    uint64_t before_weight = root->weight;

    void *picked = draw(&root);
    assert(picked != NULL);

    assert(root->count == before_count - 1);
    assert(root->weight < before_weight);

    free_tree(root);
}

void prop_draw_is_permutation(void) {
    const int N = 100;
    Node *root = NULL;
    int seen[N];
    for (int i = 0; i < N; i++) seen[i] = 0;

    for (int i = 0; i < N; i++) {
        int *payload = malloc(sizeof(int));
        *payload = i;
        root = insert(root, payload, 1);
    }

    for (int i = 0; i < N; i++) {
        int *picked = (int*)draw(&root);
        assert(*picked >= 0 && *picked < N);
        assert(seen[*picked] == 0);
        seen[*picked] = 1;
        free(picked);
    }

    assert(root == NULL);

    for (int i = 0; i < N; i++) assert(seen[i] == 1);
}

void prop_weighted_distribution(void) {
    const int TRIALS = 120000;
    int hits[3] = {0};

    for (int i = 0; i < TRIALS; i++) {
        Node *root = NULL;
        int a=0, b=1, c=2;
        root = insert(root, &a, 1);
        root = insert(root, &b, 2);
        root = insert(root, &c, 3);

        int *picked = (int*)draw(&root);
        hits[*picked]++;
        free_tree(root);
    }

    /* Check ratios ~ 1:2:3 */
    assert(abs(hits[1] * 1 - hits[0] * 2) < TRIALS * 0.01);
    assert(abs(hits[2] * 1 - hits[0] * 3) < TRIALS * 0.01);
}

#include <stdio.h>
#include <time.h>

int main(void) {
    srand((unsigned)time(NULL));

    prop_structure_random_inserts();
    prop_draw_removes_one();
    prop_draw_is_permutation();
    prop_weighted_distribution();

    printf("All property tests passed.\n");
    return 0;
}


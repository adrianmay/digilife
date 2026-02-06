#include <stdio.h>
#include <assert.h>
#include <stdint.h>

// Include the heap code here or in a header
// (Heap struct, heap_alloc, heap_free, HEAP_DEFINE, deref_<Struct>)

typedef struct {
    int x, y;
    char padding[8];
} Vec2;

// Define a heap: 2^5 = 32-byte blocks, 16 blocks
HEAP_DEFINE(Vec2, 5, 16);

// Another struct and heap for multi-heap testing
typedef struct {
    double v[2];
} Vec2d;

HEAP_DEFINE(Vec2d, 6, 8);  // 2^6 = 64-byte blocks, 8 blocks

int main(void) {
    printf("Starting heap tests...\n");

    // --- Test 1: Allocate all blocks in Vec2_heap ---
    size_t indices[16];
    for (int i = 0; i < 16; i++) {
        indices[i] = heap_alloc(&Vec2_heap);
        assert(indices[i] != (size_t)-1);
        Vec2 *p = deref_Vec2(&Vec2_heap, indices[i]);
        p->x = i;
        p->y = i*10;
    }

    // Next allocation should fail
    size_t idx = heap_alloc(&Vec2_heap);
    assert(idx == (size_t)-1);

    // --- Test 2: Free and reuse ---
    heap_free(&Vec2_heap, indices[5]);
    heap_free(&Vec2_heap, indices[10]);

    size_t r1 = heap_alloc(&Vec2_heap);
    size_t r2 = heap_alloc(&Vec2_heap);

    // Freed indices should be reused first (LIFO)
    assert(r2 == indices[5]);
    assert(r1 == indices[10]);

    // --- Test 3: Verify deref pointers ---
    Vec2 *p1 = deref_Vec2(&Vec2_heap, r1);
    Vec2 *p2 = deref_Vec2(&Vec2_heap, r2);
    p1->x = 123; p1->y = 456;
    p2->x = 789; p2->y = 987;

    assert(p1->x == 123 && p1->y == 456);
    assert(p2->x == 789 && p2->y == 987);

    // --- Test 4: High-water mark correctness ---
    assert(Vec2_heap.high_water == 16);

    // --- Test 5: Multiple heaps independent ---
    size_t v_indices[8];
    for (int i = 0; i < 8; i++) {
        v_indices[i] = heap_alloc(&Vec2d_heap);
        assert(v_indices[i] != (size_t)-1);
        Vec2d *vd = deref_Vec2d(&Vec2d_heap, v_indices[i]);
        vd->v[0] = i*1.0;
        vd->v[1] = i*2.0;
    }

    // Vec2_heap allocations should be unchanged
    Vec2 *check = deref_Vec2(&Vec2_heap, indices[0]);
    assert(check->x == 0 && check->y == 0);

    // --- Test 6: Exhaust Vec2d_heap ---
    size_t fail_idx = heap_alloc(&Vec2d_heap);
    assert(fail_idx == (size_t)-1);

    printf("All tests passed successfully!\n");
    return 0;
}

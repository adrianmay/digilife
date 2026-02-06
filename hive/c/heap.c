
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>

typedef struct Heap {
    size_t block_size;    // size of each block (power of 2)
    size_t max_blocks;    // number of blocks

    size_t high_water;    // first never-allocated block
    size_t freed_list;    // head of intrusive freed list, SIZE_MAX = empty

    uint8_t *blocks;      // pointer to static backing array
} Heap;

#define HEAP_DEFINE(type, block_exp, max_blocks)                       \
    enum { CONCAT(type,_block_size) = 1 << (block_exp) };             \
    static_assert(CONCAT(type,_block_size) >= sizeof(type), "Block size too small"); \
    static __declspec(align(4096)) uint8_t type##_heap_blocks[(max_blocks) * CONCAT(type,_block_size)]; \
    static Heap type##_heap = {                                        \
        .block_size = CONCAT(type,_block_size),                        \
        .max_blocks = (max_blocks),                                    \
        .high_water = 0,                                               \
        .freed_list = (size_t)-1,                                      \
        .blocks = type##_heap_blocks                                    \
    };                                                                  \
    static inline type *deref_##type(Heap *h, size_t index) {          \
        assert(index < h->high_water);                                  \
        return (type *)(h->blocks + index * h->block_size);            \
    }

static inline void set_next_freed(Heap *h, size_t index, size_t next) {
    *(size_t *)(h->blocks + index * h->block_size) = next;
}

static inline size_t get_next_freed(Heap *h, size_t index) {
    return *(size_t *)(h->blocks + index * h->block_size);
}

size_t heap_alloc(Heap *h) {
    size_t index;

    if (h->freed_list != (size_t)-1) {
        index = h->freed_list;
        h->freed_list = get_next_freed(h, index);
    } else {
        if (h->high_water >= h->max_blocks) return (size_t)-1;
        index = h->high_water++;
    }

    return index;
}

void heap_free(Heap *h, size_t index) {
    assert(index < h->high_water);
    set_next_freed(h, index, h->freed_list);
    h->freed_list = index;
}

typedef struct {
    int x, y;
    char padding[16]; // struct smaller than block
} Vec2;

// Block size = 32 bytes (2^5), 1024 blocks
/*
HEAP_DEFINE(Vec2, 5, 1024);

int main(void) {
    size_t a = heap_alloc(&Vec2_heap);
    size_t b = heap_alloc(&Vec2_heap);

    Vec2 *va = deref_Vec2(&Vec2_heap, a);
    Vec2 *vb = deref_Vec2(&Vec2_heap, b);

    va->x = 1; va->y = 2;
    vb->x = 3; vb->y = 4;

    printf("va = (%d,%d), vb = (%d,%d)\n", va->x, va->y, vb->x, vb->y);

    heap_free(&Vec2_heap, a);

    size_t c = heap_alloc(&Vec2_heap);
    Vec2 *vc = deref_Vec2(&Vec2_heap, c);
    printf("Reused index: %zu\n", c);

    return 0;
}
*/

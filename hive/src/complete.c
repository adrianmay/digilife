#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>

// -------------------- Heap --------------------

#if defined(_MSC_VER)
  #define PAGE_ALIGNED __declspec(align(4096))
#else
  #define PAGE_ALIGNED __attribute__((aligned(4096)))
#endif

typedef struct Heap {
    size_t block_size;    // bytes per block (power of 2)
    size_t max_blocks;    // maximum number of blocks
    size_t high_water;    // next unused block index
    size_t freed_list;    // intrusive free list head
    uint8_t *blocks;      // actual memory
} Heap;

// Free list helpers (intrusive)
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

// Macro to define a typed heap
#define CONCAT(a,b) a##b
#define HEAP_DEFINE(type, block_exp, max_blocks)                          \
    enum { CONCAT(type,_block_size) = 1 << (block_exp) };                 \
    static_assert(CONCAT(type,_block_size) >= sizeof(type), "Block size too small"); \
    static PAGE_ALIGNED uint8_t type##_heap_blocks[(max_blocks) * CONCAT(type,_block_size)]; \
    static Heap type##_heap = {                                           \
        .block_size = CONCAT(type,_block_size),                           \
        .max_blocks = (max_blocks),                                       \
        .high_water = 0,                                                  \
        .freed_list = (size_t)-1,                                         \
        .blocks = type##_heap_blocks                                      \
    };                                                                     \
    static inline type *deref_##type(Heap *h, size_t index) {             \
        assert(index < h->high_water);                                     \
        return (type *)(h->blocks + index * h->block_size);               \
    }

// -------------------- Vec2 --------------------

typedef struct {
    int x, y;
    // Kill diary fields
    uint64_t kill_tocks;
    size_t diary_index;
} Vec2;

HEAP_DEFINE(Vec2, 5, 1024) // 32-byte blocks (2^5)

// -------------------- Kill Diary --------------------

typedef struct KillDiary {
    size_t *heap_indices; // min-heap of block indices
    size_t capacity;
    size_t size;
    Heap *heap;           // underlying heap
} KillDiary;

// Swap elements in the min-heap
static inline void diary_swap(KillDiary *d, size_t i, size_t j) {
    size_t a = d->heap_indices[i];
    size_t b = d->heap_indices[j];
    d->heap_indices[i] = b;
    d->heap_indices[j] = a;
    deref_Vec2(d->heap, a)->diary_index = j;
    deref_Vec2(d->heap, b)->diary_index = i;
}

// Heap operations
void diary_sift_up(KillDiary *d, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx-1)/2;
        if (deref_Vec2(d->heap, d->heap_indices[idx])->kill_tocks >=
            deref_Vec2(d->heap, d->heap_indices[parent])->kill_tocks)
            break;
        diary_swap(d, idx, parent);
        idx = parent;
    }
}

void diary_sift_down(KillDiary *d, size_t idx) {
    size_t n = d->size;
    while (1) {
        size_t left = 2*idx + 1;
        size_t right = 2*idx + 2;
        size_t smallest = idx;

        if (left < n && deref_Vec2(d->heap, d->heap_indices[left])->kill_tocks <
                        deref_Vec2(d->heap, d->heap_indices[smallest])->kill_tocks)
            smallest = left;
        if (right < n && deref_Vec2(d->heap, d->heap_indices[right])->kill_tocks <
                        deref_Vec2(d->heap, d->heap_indices[smallest])->kill_tocks)
            smallest = right;
        if (smallest == idx) break;
        diary_swap(d, idx, smallest);
        idx = smallest;
    }
}

// Add a block to the diary
void diary_add(KillDiary *d, size_t block_index, uint64_t kill_tocks) {
    assert(d->size < d->capacity);
    Vec2 *b = deref_Vec2(d->heap, block_index);
    b->kill_tocks = kill_tocks;
    size_t idx = d->size;
    d->heap_indices[idx] = block_index;
    b->diary_index = idx;
    d->size++;
    diary_sift_up(d, idx);
}

// Remove a block from the diary
void diary_remove(KillDiary *d, size_t block_index) {
    Vec2 *b = deref_Vec2(d->heap, block_index);
    size_t idx = b->diary_index;
    if (idx >= d->size) return;
    size_t last = d->heap_indices[d->size-1];
    d->heap_indices[idx] = last;
    deref_Vec2(d->heap, last)->diary_index = idx;
    d->size--;
    diary_sift_down(d, idx);
    diary_sift_up(d, idx);
}

// Advance kill time
void diary_advance(KillDiary *d, size_t block_index, uint64_t dt) {
    Vec2 *b = deref_Vec2(d->heap, block_index);
    b->kill_tocks += dt;
    diary_sift_down(d, b->diary_index);
}

// Pop next block to delete
size_t diary_pop(KillDiary *d, uint64_t current_tocks) {
    if (d->size == 0) return (size_t)-1;
    Vec2 *top = deref_Vec2(d->heap, d->heap_indices[0]);
    if (top->kill_tocks > current_tocks) return (size_t)-1;
    size_t idx = d->heap_indices[0];
    diary_remove(d, idx);
    return idx;
}

// Initialize diary
void diary_init(KillDiary *d, Heap *heap) {
    d->heap = heap;
    d->capacity = heap->max_blocks;
    d->size = 0;
    d->heap_indices = malloc(sizeof(size_t) * heap->max_blocks);
    assert(d->heap_indices);
}

// -------------------- Example --------------------

int main(void) {
    KillDiary diary;
    diary_init(&diary, &Vec2_heap);

    uint64_t now = 0;

    // Allocate some blocks
    size_t a = heap_alloc(&Vec2_heap);
    size_t b = heap_alloc(&Vec2_heap);
    size_t c = heap_alloc(&Vec2_heap);

    printf("Allocated indices: %zu, %zu, %zu\n", a,b,c);

    diary_add(&diary,a, now+1000);
    diary_add(&diary,b, now+2000);
    diary_add(&diary,c, now+1500);

    // Advance time
    now += 1200;

    // Pop blocks due
    size_t idx;
    while ((idx = diary_pop(&diary, now)) != (size_t)-1) {
        printf("Deleting block index %zu at tock %" PRIu64 "\n", idx, now);
        heap_free(&Vec2_heap, idx);
    }

    // Postpone c (if still allocated)
    if (c < Vec2_heap.high_water) {
        diary_advance(&diary,c,1000);
    }

    now += 500;
    while ((idx = diary_pop(&diary, now)) != (size_t)-1) {
        printf("Deleting block index %zu at tock %" PRIu64 "\n", idx, now);
        heap_free(&Vec2_heap, idx);
    }

    free(diary.heap_indices);
    printf("Remaining heap high water: %zu\n", Vec2_heap.high_water);
    return 0;
}

#include "arena.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

#define MIN_MEM_REGION_SIZE_BYTES 96
// should be based on sizeof(cens_arena_t) + max_alignment_bytes supported
// 96 bytes for 32 bit, 64 bytes for 16 bit

#define ARENA_ASSERT_BOUNDS(new_head, b1, b2)                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        assert((PTRTYPE_SIZE)(new_head) <= (PTRTYPE_SIZE)(b2) && (PTRTYPE_SIZE)(new_head) >= (PTRTYPE_SIZE)(b1));      \
    } while (0)

/**
 * @brief Increments pointer by required size
 * @param Pointer to current free memory
 * @param Required Size in Bytes
 * @param Alignment Mask
 * @param Growth Direction (1 or -1)
 * @details Performs pointer arithmetic to increment pointer by required size with alignment.
 * This is a private function as it does not check for memory bounds.
 */
static PTRTYPE_SIZE __incr_head_align(PTRTYPE_SIZE, size_t, size_t, int);

// clang-format off
static inline PTRTYPE_SIZE __align_prev(PTRTYPE_SIZE v, size_t mask)
{
    return (v & ~((PTRTYPE_SIZE)mask));
}

static inline PTRTYPE_SIZE __align_next(PTRTYPE_SIZE v, size_t mask)
{
    return ((v + mask) & ~((PTRTYPE_SIZE)mask));
}
// clang-format on

struct cens_arena_t* _arena_init_impl(MEMORY_BYTE_SIZE* buffer, size_t buffer_size, struct cens_arena_options_t* opts,
                                      struct cens_arena_t* a)
{
    PTRTYPE_SIZE head_ptr       = 0;
    int          growth_dir     = DEFAULT_GROWTH_DIR;
    size_t       alignment_mask = DEFAULT_BYTE_ALIGNMENT - 1;

    assert((DEFAULT_BYTE_ALIGNMENT & (DEFAULT_BYTE_ALIGNMENT - 1)) == 0);

    if (buffer == NULL || buffer_size < MIN_MEM_REGION_SIZE_BYTES) return NULL;
    DEBUG("Buffer is not NULL and have enought space");

    if (opts != NULL)
    {
        // TODO: allow for arbitrary alignment boundary
        if (opts->byteAlignment == 32) { alignment_mask = 0x001f; }
        else if (opts->byteAlignment == 64) { alignment_mask = 0x003f; }
        else if (opts->byteAlignment == 16) { alignment_mask = 0x000f; }
        else if (opts->byteAlignment == 8) { alignment_mask = 0x0007; }
        else if (opts->byteAlignment == 4) { alignment_mask = 0x0003; }
        else if (opts->byteAlignment == 2) { alignment_mask = 0x0001; }
        else if (opts->byteAlignment == 1) { alignment_mask = 0x0000; }
        else { return NULL; }

        if (opts->growthDirection >= 0) { growth_dir = 1; }
        else { growth_dir = -1; }
    }
    DEBUG("Alignment Mask: 0x%08zX", alignment_mask);
    DEBUG("Growth Direction: %i", growth_dir);

    DEBUG("buffer points to \t[%llu]:%p", (PTRTYPE_SIZE)buffer, buffer);
    if (growth_dir > 0) { head_ptr = __align_next((PTRTYPE_SIZE)buffer, alignment_mask); }
    else
    {
        // points to the last byte of the buffer. buffer[size-1]
        head_ptr = (PTRTYPE_SIZE)(buffer + (buffer_size));
        head_ptr = __align_prev((PTRTYPE_SIZE)head_ptr, alignment_mask);
    }

    PTRTYPE_SIZE         new_head  = head_ptr;
    struct cens_arena_t* new_arena = a;
    if (new_arena == NULL)
    {
        new_head = __incr_head_align(head_ptr, sizeof(struct cens_arena_t), alignment_mask, growth_dir);

        if (growth_dir > 0) { new_arena = (struct cens_arena_t*)head_ptr; }
        else { new_arena = (struct cens_arena_t*)new_head; }
    }

    size_t available_size = 0;
    if (growth_dir > 0) { available_size = (PTRTYPE_SIZE)buffer + buffer_size - new_head; }
    else { available_size = new_head - (PTRTYPE_SIZE)buffer; }

    // since bufferSizeInBytes is already checked, not check bound here again
    // assert(new_head < ((PTRTYPE_SIZE)buffer + bufferSizeInBytes)); // for gd = 1
    // assert(new_head > buffer); // for gd = -1
    ARENA_ASSERT_BOUNDS(new_head, buffer, buffer + buffer_size);

    DEBUG("head_ptr points to \t[%llu]:%p", head_ptr, head_ptr);
    DEBUG("sizeof cens_arena_t is %llu", sizeof(struct cens_arena_t));
    DEBUG("size allocated for cens_arena_t is %llu", buffer_size - available_size);
    DEBUG("New Head points to \t[%llu]:%p", new_head, new_head);

    new_arena->buffer        = buffer;
    new_arena->bufferSize    = buffer_size;
    new_arena->availableSize = available_size;
    new_arena->headPtr       = new_head;
    new_arena->_mask         = alignment_mask;
    new_arena->_dir          = growth_dir;
    new_arena->initFlag      = 1;

    return new_arena;
}

void* _arena_malloc(struct cens_arena_t* a, size_t rsize)
{
    if (a == NULL || rsize < 1) { return NULL; }
    if (!a->initFlag) { return NULL; }

    PTRTYPE_SIZE old_head = a->headPtr;
    PTRTYPE_SIZE new_head = __incr_head_align(old_head, rsize, a->_mask, a->_dir);

    size_t allocated_size = (size_t)(new_head - old_head);
    if (a->_dir < 0) { allocated_size = (size_t)(old_head - new_head); }
    DEBUG("Required | Available | Given:\t[%llu]  |  [%llu]  |  [%llu]", rsize, a->availableSize, allocated_size);

    if (allocated_size > a->availableSize) { return NULL; }
    ARENA_ASSERT_BOUNDS(new_head, a->buffer, (PTRTYPE_SIZE)a->buffer + a->bufferSize);

    // WARNING: THIS MUST BE AN ATOMIC OPERATOIN FOR THREAD SAFETY
    a->headPtr = new_head;
    a->availableSize -= allocated_size;

    void* return_ptr = (void*)((a->_dir > 0) ? old_head : new_head);
    return return_ptr;
}

void _arena_destroy(struct cens_arena_t* a)
{
    a->initFlag      = 0;
    a->availableSize = 0;
    return;
}

static PTRTYPE_SIZE __incr_head_align(PTRTYPE_SIZE _h, size_t _rqs, size_t _am, int _gd)
{
    PTRTYPE_SIZE _nh = _h;
    if (_gd > 0)
    {
        _nh += (PTRTYPE_SIZE)_rqs;
        return __align_next(_nh, _am);
    }
    else
    {
        _nh -= (PTRTYPE_SIZE)_rqs;
        return __align_prev(_nh, _am);
    }
}

// clang-format off
// references
// 1. [FreeRTOS Heap Implementations](https://github.com/FreeRTOS/FreeRTOS-Kernel/tree/main/portable/MemMang)
// 2. [FreeRTOS Portable](https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/include/portable.h)
// 3. [FreeRTOS Task](https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/tasks.c)
// 4. [Everyone is doing memory management wrong. feat. Ryan Fleury | S2 E02](https://youtu.be/UeJPyuVxL-o?si=XWllRaMBqSx0p1OC)
// 5. [Game Engine Stream: Creating a Linear Memory Allocator](https://www.youtube.com/live/FqeanKu3NzI?si=ot4sPJyb-D40TWEP)
// 6. [Memory Allocation Strategies - Part 2](https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/)

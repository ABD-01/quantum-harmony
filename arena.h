#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>

#ifdef ENABLE_DEBUG
#include <stdio.h>
#define DEBUG(fmt, ...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt "\r\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define DEBUG(fmt, ...) ((void)0)
#endif

#define MEMORY_BYTE_SIZE uint8_t
#define PTRTYPE_SIZE     uintptr_t

#ifndef DEFAULT_BYTE_ALIGNMENT
#define DEFAULT_BYTE_ALIGNMENT (32)
#endif

#ifndef DEFAULT_GROWTH_DIR
#define DEFAULT_GROWTH_DIR (1)
#endif

struct cens_arena_t
{
    size_t            bufferSize;
    size_t            availableSize;
    size_t            initFlag;
    size_t            _mask;
    int               _dir;
    PTRTYPE_SIZE      headPtr; /**< Moves along with the allocations */
    MEMORY_BYTE_SIZE* buffer;  /**< Fixed at the begining of the memory */
};

struct cens_arena_options_t
{
    uint8_t byteAlignment;
    int8_t  growthDirection; /**< 1 = forward (higher addresses), -1 = reverse (lower addresses) */
};

/**
 * @brief Initializes the Arena
 * @param Memory Buffer to be used
 * @param Size of the Memory buffer provided
 * @param Options
 * @TODO other possibility is to pass a pointer to arena, and update it
 * @return Pointer to initialized arena structure.
 */
struct cens_arena_t* _arena_init_impl(MEMORY_BYTE_SIZE*, size_t, struct cens_arena_options_t*, struct cens_arena_t*);

// thread un-safe!!
void* _arena_malloc(struct cens_arena_t*, size_t);
void  _arena_destroy(struct cens_arena_t*);

/**
 * @brief Creates a new arena allocator instance.
 * @param Pointer to the memory buffer provided by the user.
 * @param Size of the Memory buffer provided
 * @return Pointer to the initialized arena, or NULL on failure.
 * @note The arena struct is embedded in the buffer. use cens_allocator_init
 * if caller want the arena metadata stored outside the provided buffer.
 */
static inline struct cens_arena_t* cens_allocator_new(MEMORY_BYTE_SIZE* buffer, size_t bufferSizeInBytes)
{
    struct cens_arena_options_t opts = {DEFAULT_BYTE_ALIGNMENT, DEFAULT_GROWTH_DIR};
    return _arena_init_impl(buffer, bufferSizeInBytes, &opts, NULL);
}

/**
 * @brief Initializes a user-provided arena struct with the given buffer.
 * @param [out] Pointer to an already allocated arena struct.
 * @param Pointer to the memory buffer provided by the user.
 * @param Size of the Memory buffer provided
 * @return 0 on success, 1 on failure
 */
static inline int cens_allocator_init(struct cens_arena_t* arena, MEMORY_BYTE_SIZE* buffer, size_t bufferSizeInBytes)
{
    if (arena == NULL) { return 1; }
    struct cens_arena_options_t opts = {DEFAULT_BYTE_ALIGNMENT, DEFAULT_GROWTH_DIR};
    if (_arena_init_impl(buffer, bufferSizeInBytes, &opts, arena) == NULL) { return 1; }
    return 0;
}

/**
 * @brief Allocates memory from the arena.
 * @param arena   Pointer to an initialized arena allocator.
 * @param size    Size of memory to allocate in bytes.
 * @return Pointer to allocated memory, or NULL if allocation fails.
 * @warning This function is **not thread-safe**.
 * @code
 * __disable_irq();
 * void* ptr = cens_allocator_malloc(arena, sizeof(int) * 4);
 * __enable_irq();
 * @endcode
 */
static inline void* cens_allocator_malloc(struct cens_arena_t* a, size_t requiredSizeInBytes)
{
    return _arena_malloc(a, requiredSizeInBytes);
}

/**
 * @brief Dummy free function (does nothing).
 */
static inline void cens_allocator_free(struct cens_arena_t* a, void* ptr) { return; }

/**
 * @brief Destructor
 */
static inline void cens_allocator_destroy(struct cens_arena_t* a) { _arena_destroy(a); }

#endif // ARENA_H

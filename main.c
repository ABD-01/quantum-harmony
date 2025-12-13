#include "arena.h"
#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>

struct foo
{
    int  a;
    char b;
    char c[3];
};

int main(int argc, char* argv[])
{
#define BUF_SIZE (129)
    char buf[BUF_SIZE];
    DEBUG("buf starts at \t[%llu]: %p", (PTRTYPE_SIZE)&buf[0], &buf[0]);

    struct cens_arena_t* a = _arena_init_impl((MEMORY_BYTE_SIZE*)buf, BUF_SIZE, &(struct cens_arena_options_t){16, -1}, NULL);
    //struct cens_arena_t my_arena = {0};
    //if (cens_allocator_init(&my_arena, (MEMORY_BYTE_SIZE*)buf, BUF_SIZE)) { printf("Arena Init Failed"); }

    // struct cens_arena_t* a = &my_arena;

    if (a == NULL)
    {
        printf("Arena Init Failed");
        return 1;
    }

    DEBUG("Arena Initialized Successfully");

    DEBUG("Arena->buffer:\t\t[%llu]:%p", a->buffer, a->buffer);
    DEBUG("Arena->buferSize:\t\t[%llu]:%p", a->bufferSize, a->bufferSize);
    DEBUG("Arena->availableSize:\t\t[%llu]:%p", a->availableSize, a->availableSize);
    DEBUG("Arena->headPtr:\t\t[%llu]:%p", a->headPtr, a->headPtr);
    DEBUG("Arena->initFlag:\t\t[%llu]:%p", a->initFlag, a->initFlag);
    DEBUG("Arena->alignment_mask:\t\t[%llu]:%p", a->_mask, a->_mask);
    DEBUG("Arena->growth_dir:\t\t[%llu]:%p", a->_dir, a->_dir);

    struct foo* my_foo = _arena_malloc(a, 56);
    if (my_foo == NULL)
    {
        printf("malloc failed\n");
        return 1;
    }
    DEBUG("Malloc Successful");

    DEBUG("Arena->availableSize:\t\t[%llu]:%p", a->availableSize, a->availableSize);
    DEBUG("Arena->headPtr:\t\t[%llu]:%p", a->headPtr, a->headPtr);

    _arena_destroy(a);

    DEBUG("Arena->availableSize:\t\t[%llu]:%p", a->availableSize, a->availableSize);
    DEBUG("Arena->headPtr:\t\t[%llu]:%p", a->headPtr, a->headPtr);

    return 0;
}

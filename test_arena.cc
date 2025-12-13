#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>

extern "C"
{
#include "arena.h"
}

#define MIN_SIZE 96

class ArenaTest : public ::testing::TestWithParam<int>
{
  protected:
    static constexpr size_t BUF_SIZE = 1024;
    alignas(64) char buffer[BUF_SIZE];
    cens_arena_options_t opts;
    cens_arena_t*        arena;

    uintptr_t align_up(uintptr_t p, size_t mask) { return (p + mask) & ~mask; }
    uintptr_t align_down(uintptr_t p, size_t mask) { return p & ~mask; }

    void init_arena(int dir = 1, size_t size = BUF_SIZE, void* buf = nullptr)
    {
        int align            = GetParam(); // e.g., 16 or 32
        opts.byteAlignment   = align;
        opts.growthDirection = dir;
        void* memory         = buf ? buf : buffer;
        arena                = _arena_init_impl((MEMORY_BYTE_SIZE*)memory, size, &opts, NULL);
    }

    void TearDown() override
    {
        if (arena) _arena_destroy(arena);
    }
};

TEST_P(ArenaTest, Basic)
{
    size_t buffer_size = 1024;
    opts               = {.byteAlignment = 16, .growthDirection = 1};

    arena = _arena_init_impl((MEMORY_BYTE_SIZE*)buffer, 1024, &opts, NULL);
    EXPECT_NE(arena, nullptr);
    EXPECT_EQ(arena->initFlag, 1);

    EXPECT_EQ((PTRTYPE_SIZE)arena->buffer, (PTRTYPE_SIZE)buffer);
    uintptr_t expected_new_head = (PTRTYPE_SIZE)buffer + sizeof(struct cens_arena_t);
    expected_new_head           = (expected_new_head + (opts.byteAlignment - 1)) & (~(opts.byteAlignment - 1));
    EXPECT_EQ(arena->headPtr, expected_new_head);
}

TEST_P(ArenaTest, InitFailsOnSmallBuffer)
{
    init_arena(1, MIN_SIZE - 1);
    EXPECT_EQ(arena, nullptr);
}

TEST_P(ArenaTest, InitFailsOnBadAlignment)
{
    init_arena(1, 7); // Not power of 2
    EXPECT_EQ(arena, nullptr);
}

TEST_P(ArenaTest, InitFailsOnNullBuffer)
{
    init_arena(1, BUF_SIZE, nullptr);
    arena = _arena_init_impl(nullptr, BUF_SIZE, &opts, NULL);
    EXPECT_EQ(arena, nullptr);
}

TEST_P(ArenaTest, InitSuccessPositiveGrowth)
{
    init_arena(+1);
    ASSERT_NE(arena, nullptr);

    uintptr_t start               = reinterpret_cast<uintptr_t>(buffer);
    uintptr_t expected_struct_ptr = align_up(start, arena->_mask);
    uintptr_t actual_struct_ptr   = reinterpret_cast<uintptr_t>(arena);

    EXPECT_EQ(actual_struct_ptr, expected_struct_ptr);

    uintptr_t next_head = align_up(expected_struct_ptr + sizeof(cens_arena_t), arena->_mask);
    EXPECT_EQ(arena->headPtr, next_head);

    EXPECT_EQ(arena->_dir, 1);
}

TEST_P(ArenaTest, InitSuccessNegativeGrowth)
{
    init_arena(-1);
    ASSERT_NE(arena, nullptr);

    uintptr_t end                = reinterpret_cast<uintptr_t>(buffer + BUF_SIZE - sizeof(cens_arena_t));
    uintptr_t expected_last_byte = align_down(end, arena->_mask);
    EXPECT_EQ(arena->headPtr, expected_last_byte);

    EXPECT_EQ(arena->_dir, -1);
}

TEST_P(ArenaTest, MallocSuccessReducesAvailable)
{
    init_arena(+1);
    ASSERT_NE(arena, nullptr);

    uintptr_t old_head = arena->headPtr;
    size_t    rsize    = 64;

    void* ptr = _arena_malloc(arena, rsize);
    ASSERT_NE(ptr, nullptr);

    uintptr_t expected_new_head = align_up(old_head + rsize, arena->_mask);
    EXPECT_EQ(arena->headPtr, expected_new_head);

    size_t expected_available = BUF_SIZE - (expected_new_head - (uintptr_t)buffer);
    EXPECT_EQ(arena->availableSize, expected_available);
}

TEST_P(ArenaTest, MallocFailOversize)
{
    init_arena(+1);
    ASSERT_NE(arena, nullptr);

    void* ptr = _arena_malloc(arena, BUF_SIZE * 2);
    EXPECT_EQ(ptr, nullptr);
}

TEST_P(ArenaTest, MallocFailZeroRequest)
{
    init_arena(+1);
    ASSERT_NE(arena, nullptr);

    void* ptr = _arena_malloc(arena, 0);
    EXPECT_EQ(ptr, nullptr);
}

TEST_P(ArenaTest, MallocNegativeGrowthSuccess)
{
    init_arena(-1);
    ASSERT_NE(arena, nullptr);

    uintptr_t old_head = arena->headPtr;
    size_t    rsize    = 128;

    void* ptr = _arena_malloc(arena, rsize);
    ASSERT_NE(ptr, nullptr);

    uintptr_t expected_new_head = align_down(old_head - rsize, arena->_mask);
    EXPECT_EQ(arena->headPtr, expected_new_head);

    size_t expected_available = old_head - expected_new_head;
    EXPECT_EQ(arena->availableSize, BUF_SIZE - expected_available);
}

TEST_P(ArenaTest, InitUnalignedBufferStillAlignsCorrectly)
{
    char  raw_buf[BUF_SIZE + 1];
    char* unaligned = raw_buf + 1; // Likely unaligned
    init_arena(+1, BUF_SIZE, unaligned);

    ASSERT_NE(arena, nullptr);
    uintptr_t ptr = reinterpret_cast<uintptr_t>(arena);
    EXPECT_EQ(ptr % 16, 0);
}

INSTANTIATE_TEST_SUITE_P(Alignments, ArenaTest, ::testing::Values(16, 32, 64) // easily add more alignments
);

/*
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
*/

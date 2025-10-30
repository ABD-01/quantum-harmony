# Logical View: UTIL API Reference

**Parent:** [Utilities Module Hub](logical_view_util.md) | [Logical View](logical_view.md) | [Architecture Home](README.md)

## Table of Contents
- [1. Memory Fill Operations](#1-memory-fill-operations)
- [2. Memory Copy Operations](#2-memory-copy-operations)
- [3. Helper Functions](#3-helper-functions)
- [4. Usage Examples](#4-usage-examples)
- [5. References](#5-references)

---

## 1. Memory Fill Operations

### asl_util__buffer_memset()

Fills a buffer with a specified value using type-safe operations.

**Signature**:
```c
void asl_util__buffer_memset(asl_buffer_s dest, uint8_t value);
```

**Parameters**:
- `dest`: Destination buffer structure
- `value`: Byte value to fill with

**Behavior**:
- Validates buffer pointer and size internally
- Fills entire buffer with specified value
- Performs efficient memory fill operation

**Example**:
```c
uint8_t memory[1024];
asl_buffer_s buffer = { .ptr = memory, .size = sizeof(memory) };

// Fill buffer with zeros
asl_util__buffer_memset(buffer, 0x00);

// Fill with pattern
asl_util__buffer_memset(buffer, 0xAA);
```

**Safety Features**:
- Internal bounds checking
- Operates on entire buffer size
- Type-safe buffer structure usage

---

## 2. Memory Copy Operations

### asl_util__buffer_memcpy()

Copies data between buffers with internal bounds checking.

**Signature**:
```c
void asl_util__buffer_memcpy(asl_buffer_s dest, asl_buffer_s src);
```

**Parameters**:
- `dest`: Destination buffer structure
- `src`: Source buffer structure

**Behavior**:
- Copies data from source buffer to destination buffer
- Uses the smaller of the two buffer sizes for safe copying
- Performs efficient memory copy operation
- Internal validation and bounds checking

**Features**:
- Bounds checking and size validation
- Overlap detection with automatic handling
- Efficient platform-specific implementations

---

## 3. Helper Functions

### Available Functions

The UTIL module provides buffer manipulation functions as defined in the actual implementation:

**Buffer Operations** (from `asl_util.h`):
```c
// Actual function signatures from codebase
void asl_util__buffer_memset(asl_buffer_s dest, uint8_t value);
void asl_util__buffer_memcpy(asl_buffer_s dest, asl_buffer_s src);
```

**Function Categories**:
1. **Math Operations**: min, max, clamp, absolute value
2. **Validation**: Buffer integrity, range checking, parameter validation
3. **Conversion**: Type-safe casting, endian conversion
4. **Debug Support**: Memory dumping, state inspection

---

## 4. Usage Examples

### Data Structure Initialization

```c
// Initialize circular buffer with zeros
void init_cbuf_memory(asl_cbuf_s* cbuf) {
    asl_buffer_s buffer = {
        .ptr = cbuf->mem,
        .size = cbuf->size_mem
    };
    
    // Clear buffer memory
    asl_util__buffer_memset(&buffer, 0x00, buffer.size);
    
    // Reset indices
    cbuf->front = 0;
    cbuf->rear = 0;
}
```

### Safe Data Transfer

```c
// Transfer data between data structures
bool transfer_data(asl_buffer_s* from, asl_buffer_s* to, size_t amount) {
    // Validate parameters
    if (!from || !to || amount == 0) {
        return false;
    }
    
    // Check available space
    if (to->size < amount) {
        amount = to->size;  // Clamp to available space
    }
    
    // Perform safe copy
    return asl_util__buffer_memcpy(to, from, amount);
}
```

### Pattern Fill for Testing

```c
// Fill buffer with test pattern for debugging
void fill_test_pattern(asl_buffer_s* buffer) {
    // Fill with incremental pattern
    for (size_t i = 0; i < buffer->size; i++) {
        buffer->ptr[i] = (uint8_t)(i & 0xFF);
    }
    
    // Or use utility function with repeated pattern
    asl_util__buffer_memset(buffer, 0x55, buffer->size);
}
```

### Memory Comparison and Validation

```c
// Validate buffer contains expected pattern
bool validate_buffer_pattern(const asl_buffer_s* buffer, uint8_t pattern) {
    for (size_t i = 0; i < buffer->size; i++) {
        if (buffer->ptr[i] != pattern) {
            return false;
        }
    }
    return true;
}

// Clear sensitive data
void secure_clear_buffer(asl_buffer_s* buffer) {
    asl_util__buffer_memset(buffer, 0x00, buffer->size);
    
    // Additional security: multiple passes
    asl_util__buffer_memset(buffer, 0xFF, buffer->size);
    asl_util__buffer_memset(buffer, 0x00, buffer->size);
}
```

---

## 5. References

### Related Documentation
- [UTIL Overview](logical_view_util_overview.md) - Core concepts and design
- [Usage Patterns](logical_view_util_patterns.md) - Integration patterns

### Implementation Files
- **Header**: `asl/library/asl_util.h` - Function declarations
- **Source**: `asl/library/asl_util.c` - Implementation

### Performance Notes
- **Alignment**: Functions optimize for word-aligned access
- **Bulk Operations**: Use platform-specific optimizations
- **Cache Efficiency**: Access patterns designed for cache locality
- **Compiler Intrinsics**: May use compiler-specific optimizations

### Safety Considerations
- Always check return values
- Validate buffer parameters before operations
- Consider buffer overlap in copy operations
- Use appropriate patterns for security-sensitive data clearing

# ASL (Application Support Library) Requirements Specification

## 1. System Overview

The Application Support Library (ASL) provides a set of portable data structures and utilities for embedded and desktop applications. The library implements core abstractions for memory management and common data structures with platform-independent interfaces.

The ASL library encompasses:
- Core type system with C99 standard type abstractions
- Circular buffer (CBUF) for single producer-consumer scenarios
- Generic FIFO queue with configurable element size
- Generic LIFO stack with configurable element size
- Memory buffer utility functions

---

## 2. Functional Requirements

#### ASL_REQ_001
- **Requirement ID**: ASL_REQ_001
- **Objective**: Provide platform-independent type definitions
- **Requirement**: The library SHALL define C99-compliant type aliases including asl_buffer_s, asl_allocator_t, and asl_mutex_t structures
- **Pre-Condition**: C99 compiler environment with stdint.h, stdbool.h, and stddef.h available
- **Input**: C99 standard type definitions from system headers
- **Output**: Platform-independent ASL type definitions (asl_buffer_s, asl_allocator_t, asl_mutex_t structures)
- **Acceptance Criteria**: 
  - asl_buffer_s contains ptr and size fields for memory encapsulation
  - asl_allocator_t contains alloc_f and free_f function pointers
  - asl_mutex_t contains lock_f and unlock_f function pointers
  - All structures use C99 standard types (stdint.h, stdbool.h, stddef.h)

#### ASL_REQ_002
- **Requirement ID**: ASL_REQ_002
- **Objective**: Provide function pointer type abstractions
- **Requirement**: The library SHALL define comprehensive function pointer types for common signatures
- **Pre-Condition**: Basic C99 types and interface type definitions are available
- **Input**: Standard C99 function signature patterns (void, pvoid, size_t, uint8_t, uint16_t, uint32_t, uint64_t)
- **Output**: Standardized function pointer type definitions following asl_iface_<return>_<param>_t naming convention
- **Acceptance Criteria**:
  - Function pointer types follow naming convention asl_iface_<return>_<param>_t (e.g., asl_iface_v_v_t, asl_iface_pv_s_t)
  - Support for void (v), pvoid (pv), size_t (s) return types
  - Support for various parameter combinations (void, pvoid, size_t, uint8_t, uint16_t, uint32_t, uint64_t)
  - Parameter types abbreviated: v=void, pv=pvoid, s=size_t, u8=uint8_t, u16=uint16_t, u32=uint32_t, u64=uint64_t

#### ASL_REQ_003
- **Requirement ID**: ASL_REQ_003
- **Objective**: Define pointer type aliases
- **Requirement**: The library SHALL provide pointer type aliases for standard C99 types
- **Pre-Condition**: C99 standard integer types are defined
- **Input**: Standard C99 integer types (uint8_t, uint16_t, uint32_t, uint64_t, size_t, etc.)
- **Output**: Pointer type aliases with 'p' prefix naming convention (puint8_t, psize_t, pvoid, etc.)
- **Acceptance Criteria**:
  - Pointer types defined for all standard integer types
  - Naming convention uses 'p' prefix (e.g., puint8_t, psize_t)
  - pvoid type defined as void pointer alias

#### ASL_REQ_004
- **Requirement ID**: ASL_REQ_004
- **Objective**: Implement circular buffer data structure
- **Requirement**: The CBUF module SHALL provide a circular buffer for 8-bit data elements
- **Pre-Condition**: ASL type system is available and memory allocation is possible
- **Input**: Memory buffer allocation for circular buffer storage
- **Output**: asl_cbuf_s structure instance for circular buffer operations
- **Acceptance Criteria**:
  - asl_cbuf_s structure contains mem, size_mem, front, rear fields
  - Buffer operates on uint8_t data elements only
  - Supports wrap-around indexing for circular operation

#### ASL_REQ_005
- **Requirement ID**: ASL_REQ_005
- **Objective**: Provide CBUF initialization function
- **Requirement**: The CBUF module SHALL provide asl_cbuf_init() function to reset buffer state
- **Pre-Condition**: asl_cbuf_s structure is allocated and available
- **Input**: Pointer to asl_cbuf_s structure (asl_cbuf_s* ptr_cbuf)
- **Output**: asl_cbuf_error_n return code indicating success or failure
- **Acceptance Criteria**:
  - Function signature: asl_cbuf_error_n asl_cbuf_init(asl_cbuf_s* ptr_cbuf)
  - Returns ASL_CBUF_E_OK on success
  - Returns ASL_CBUF_E_PARAM for NULL pointer input

#### ASL_REQ_006
- **Requirement ID**: ASL_REQ_006
- **Objective**: Support bulk enqueue operations
- **Requirement**: The CBUF module SHALL provide asl_cbuf_enqueue() for adding multiple bytes
- **Pre-Condition**: Initialized asl_cbuf_s structure with sufficient available space
- **Input**: asl_cbuf_s* ptr_cbuf, asl_buffer_s buffer, size_t req_enqueue
- **Output**: asl_cbuf_error_n return code and updated buffer state
- **Acceptance Criteria**:
  - Function signature: asl_cbuf_error_n asl_cbuf_enqueue(asl_cbuf_s* ptr_cbuf, asl_buffer_s buffer, size_t req_enqueue)
  - Returns ASL_CBUF_E_FORBID when insufficient space available
  - Supports atomic bulk operations

#### ASL_REQ_007
- **Requirement ID**: ASL_REQ_007
- **Objective**: Support bulk dequeue operations  
- **Requirement**: The CBUF module SHALL provide asl_cbuf_dequeue() for removing multiple bytes
- **Pre-Condition**: Initialized asl_cbuf_s structure with sufficient available data
- **Input**: asl_cbuf_s* ptr_cbuf, asl_buffer_s buffer, size_t req_dequeue
- **Output**: asl_cbuf_error_n return code and extracted data in buffer
- **Acceptance Criteria**:
  - Function signature: asl_cbuf_error_n asl_cbuf_dequeue(asl_cbuf_s* ptr_cbuf, asl_buffer_s buffer, size_t req_dequeue)
  - Returns ASL_CBUF_E_FORBID when insufficient data available
  - Removes data from buffer

#### ASL_REQ_008
- **Requirement ID**: ASL_REQ_008
- **Objective**: Support non-destructive preview operations
- **Requirement**: The CBUF module SHALL provide asl_cbuf_preview() for reading without removing data
- **Pre-Condition**: Initialized asl_cbuf_s structure with available data
- **Input**: asl_cbuf_s* ptr_cbuf, asl_buffer_s buffer, size_t req_preview
- **Output**: asl_cbuf_error_n return code and preview data in buffer (buffer state unchanged)
- **Acceptance Criteria**:
  - Function signature: asl_cbuf_error_n asl_cbuf_preview(asl_cbuf_s* ptr_cbuf, asl_buffer_s buffer, size_t req_preview)
  - Does not modify buffer state
  - Returns same data as equivalent dequeue would

#### ASL_REQ_009
- **Requirement ID**: ASL_REQ_009
- **Objective**: Provide buffer status queries
- **Requirement**: The CBUF module SHALL provide functions to query available read and write space
- **Pre-Condition**: Initialized asl_cbuf_s structure
- **Input**: asl_cbuf_s* ptr_cbuf and size_t* output parameters
- **Output**: asl_cbuf_error_n return code and size_t values via output parameters
- **Acceptance Criteria**:
  - asl_cbuf_available_read() returns asl_cbuf_error_n and provides bytes available for reading via output parameter
  - asl_cbuf_available_write() returns asl_cbuf_error_n and provides bytes available for writing via output parameter
  - Both functions follow pattern: asl_cbuf_error_n func(asl_cbuf_s* ptr_cbuf, size_t* output)

#### ASL_REQ_010
- **Requirement ID**: ASL_REQ_010
- **Objective**: Define comprehensive error codes
- **Requirement**: The CBUF module SHALL use asl_cbuf_error_n enumeration for all error conditions
- **Pre-Condition**: C99 enumeration support available
- **Input**: CBUF operation conditions requiring error classification
- **Output**: asl_cbuf_error_n enumeration values for error handling
- **Acceptance Criteria**:
  - ASL_CBUF_E_OK for successful operations
  - ASL_CBUF_E_PARAM for invalid parameters
  - ASL_CBUF_E_FORBID for operation constraints
  - 32-bit enumeration values

#### ASL_REQ_011
- **Requirement ID**: ASL_REQ_011
- **Objective**: Implement generic FIFO queue
- **Requirement**: The FIFO module SHALL provide a first-in-first-out queue for fixed-size elements
- **Pre-Condition**: ASL type system is available and memory allocation is possible
- **Input**: Element size specification and memory buffer for queue storage
- **Output**: asl_fifo_s structure instance for FIFO queue operations
- **Acceptance Criteria**:
  - asl_fifo_s structure contains elements (asl_buffer_s), size_element (size_t), index_write (size_t), index_read (int) fields
  - Supports arbitrary element sizes configured at initialization
  - Maintains FIFO ordering
  - Maintains FIFO ordering

#### ASL_REQ_012
- **Requirement ID**: ASL_REQ_012
- **Objective**: Provide FIFO reset function
- **Requirement**: The FIFO module SHALL provide asl_fifo_reset() to initialize queue state
- **Pre-Condition**: asl_fifo_s structure is allocated and available
- **Input**: Pointer to asl_fifo_s structure (asl_fifo_s* ptr_fifo)
- **Output**: asl_fifo_error_e return code indicating success or failure
- **Acceptance Criteria**:
  - Function signature: asl_fifo_error_e asl_fifo_reset(asl_fifo_s* ptr_fifo)
  - Resets read and write indices
  - Returns appropriate error codes

#### ASL_REQ_013
- **Requirement ID**: ASL_REQ_013
- **Objective**: Support capacity queries
- **Requirement**: The FIFO module SHALL provide functions to query capacity, used, and free element counts
- **Pre-Condition**: Initialized asl_fifo_s structure
- **Input**: asl_fifo_s* ptr_fifo and size_t* output parameters
- **Output**: asl_fifo_error_e return code and size_t values via output parameters
- **Acceptance Criteria**:
  - asl_fifo_get_count_capacity() returns asl_fifo_error_e and provides total element capacity via output parameter
  - asl_fifo_get_count_used() returns asl_fifo_error_e and provides currently used elements via output parameter
  - asl_fifo_get_count_free() returns asl_fifo_error_e and provides available element slots via output parameter

#### ASL_REQ_014
- **Requirement ID**: ASL_REQ_014
- **Objective**: Support element enqueue operations
- **Requirement**: The FIFO module SHALL provide asl_fifo_enqueue() to add elements
- **Pre-Condition**: Initialized asl_fifo_s structure with available capacity
- **Input**: asl_fifo_s* ptr_fifo, asl_buffer_s element
- **Output**: asl_fifo_error_e return code and element added to queue
- **Acceptance Criteria**:
  - Function signature: asl_fifo_error_e asl_fifo_enqueue(asl_fifo_s* ptr_fifo, asl_buffer_s element)
  - Returns ASL_FIFO_E_FULL when queue is full
  - Copies element data into queue

#### ASL_REQ_015
- **Requirement ID**: ASL_REQ_015
- **Objective**: Support element dequeue operations
- **Requirement**: The FIFO module SHALL provide asl_fifo_dequeue() to remove elements
- **Pre-Condition**: Initialized asl_fifo_s structure with available elements
- **Input**: asl_fifo_s* ptr_fifo, asl_buffer_s* element
- **Output**: asl_fifo_error_e return code and extracted element data
- **Acceptance Criteria**:
  - Function signature: asl_fifo_error_e asl_fifo_dequeue(asl_fifo_s* ptr_fifo, asl_buffer_s* element)
  - Returns ASL_FIFO_E_EMPTY when queue is empty
  - Removes element from queue

#### ASL_REQ_016
- **Requirement ID**: ASL_REQ_016
- **Objective**: Support non-destructive preview
- **Requirement**: The FIFO module SHALL provide asl_fifo_preview() to read without removing
- **Pre-Condition**: Initialized asl_fifo_s structure with available elements
- **Input**: asl_fifo_s* ptr_fifo, asl_buffer_s* element
- **Output**: asl_fifo_error_e return code and preview element data (queue state unchanged)
- **Acceptance Criteria**:
  - Function signature: asl_fifo_error_e asl_fifo_preview(asl_fifo_s* ptr_fifo, asl_buffer_s* element)
  - Does not modify queue state
  - Returns next element that would be dequeued

#### ASL_REQ_017
- **Requirement ID**: ASL_REQ_017
- **Objective**: Define FIFO error codes
- **Requirement**: The FIFO module SHALL use asl_fifo_error_e enumeration for error handling
- **Pre-Condition**: C99 enumeration support available
- **Input**: FIFO operation conditions requiring error classification
- **Output**: asl_fifo_error_e enumeration values for error handling
- **Acceptance Criteria**:
  - ASL_FIFO_E_OK, ASL_FIFO_E_PARAM, ASL_FIFO_E_ILLEGAL
  - ASL_FIFO_E_EMPTY, ASL_FIFO_E_FULL
  - 32-bit enumeration values

#### ASL_REQ_018
- **Requirement ID**: ASL_REQ_018
- **Objective**: Implement generic LIFO stack
- **Requirement**: The LIFO module SHALL provide a last-in-first-out stack for fixed-size elements
- **Pre-Condition**: ASL type system is available and memory allocation is possible
- **Input**: Element size specification and memory buffer for stack storage
- **Output**: asl_lifo_s structure instance for LIFO stack operations
- **Acceptance Criteria**:
  - asl_lifo_s structure contains size_element (size_t), elements (asl_buffer_s), top (size_t) fields in that order
  - Supports arbitrary element sizes configured at initialization
  - Maintains LIFO ordering

#### ASL_REQ_019
- **Requirement ID**: ASL_REQ_019
- **Objective**: Provide LIFO reset function
- **Requirement**: The LIFO module SHALL provide asl_lifo_reset() to initialize stack state
- **Pre-Condition**: asl_lifo_s structure is allocated and available
- **Input**: Pointer to asl_lifo_s structure (asl_lifo_s* ptr_lifo)
- **Output**: asl_lifo_error_e return code indicating success or failure
- **Acceptance Criteria**:
  - Function signature: asl_lifo_error_e asl_lifo_reset(asl_lifo_s* ptr_lifo)
  - Sets top index to zero (empty state)
  - Debug fills elements buffer

#### ASL_REQ_020
- **Requirement ID**: ASL_REQ_020
- **Objective**: Support capacity queries
- **Requirement**: The LIFO module SHALL provide functions to query capacity, used, and free element counts
- **Pre-Condition**: Initialized asl_lifo_s structure
- **Input**: asl_lifo_s* ptr_lifo and size_t* output parameters
- **Output**: asl_lifo_error_e return code and size_t values via output parameters
- **Acceptance Criteria**:
  - asl_lifo_get_count_capacity() returns asl_lifo_error_e and provides total element capacity via output parameter
  - asl_lifo_get_count_used() returns asl_lifo_error_e and provides currently used elements via output parameter
  - asl_lifo_get_count_free() returns asl_lifo_error_e and provides available element slots via output parameter

#### ASL_REQ_021
- **Requirement ID**: ASL_REQ_021
- **Objective**: Support element push operations
- **Requirement**: The LIFO module SHALL provide asl_lifo_push() to add elements
- **Pre-Condition**: Initialized asl_lifo_s structure with available capacity
- **Input**: asl_lifo_s* ptr_lifo, asl_buffer_s element
- **Output**: asl_lifo_error_e return code and element added to stack
- **Acceptance Criteria**:
  - Function signature: asl_lifo_error_e asl_lifo_push(asl_lifo_s* ptr_lifo, asl_buffer_s element)
  - Returns ASL_LIFO_E_FULL when stack is full
  - Increments top index after successful push

#### ASL_REQ_022
- **Requirement ID**: ASL_REQ_022
- **Objective**: Support element pop operations
- **Requirement**: The LIFO module SHALL provide asl_lifo_pop() to remove elements
- **Pre-Condition**: Initialized asl_lifo_s structure with available elements
- **Input**: asl_lifo_s* ptr_lifo, asl_buffer_s* element
- **Output**: asl_lifo_error_e return code and extracted element data
- **Acceptance Criteria**:
  - Function signature: asl_lifo_error_e asl_lifo_pop(asl_lifo_s* ptr_lifo, asl_buffer_s* element)
  - Returns ASL_LIFO_E_EMPTY when stack is empty
  - Decrements top index after successful pop

#### ASL_REQ_023
- **Requirement ID**: ASL_REQ_023
- **Objective**: Support non-destructive peek
- **Requirement**: The LIFO module SHALL provide asl_lifo_peek() to read without removing
- **Pre-Condition**: Initialized asl_lifo_s structure with available elements
- **Input**: asl_lifo_s* ptr_lifo, asl_buffer_s* element
- **Output**: asl_lifo_error_e return code and peek element data (stack state unchanged)
- **Acceptance Criteria**:
  - Function signature: asl_lifo_error_e asl_lifo_peek(asl_lifo_s* ptr_lifo, asl_buffer_s* element)
  - Does not modify stack state
  - Returns top element that would be popped

#### ASL_REQ_024
- **Requirement ID**: ASL_REQ_024
- **Objective**: Define LIFO error codes
- **Requirement**: The LIFO module SHALL use asl_lifo_error_e enumeration for error handling
- **Pre-Condition**: C99 enumeration support available
- **Input**: LIFO operation conditions requiring error classification
- **Output**: asl_lifo_error_e enumeration values for error handling
- **Acceptance Criteria**:
  - ASL_LIFO_E_OK, ASL_LIFO_E_PARAM, ASL_LIFO_E_ILLEGAL
  - ASL_LIFO_E_EMPTY, ASL_LIFO_E_FULL
  - 32-bit enumeration values

#### ASL_REQ_025
- **Requirement ID**: ASL_REQ_025
- **Objective**: Provide buffer memory set function
- **Requirement**: The utility module SHALL provide asl_util_buffer_memset() for filling buffers
- **Pre-Condition**: Valid asl_buffer_s destination buffer with allocated memory
- **Input**: asl_buffer_s dest, uint8_t value
- **Output**: Destination buffer filled with specified byte value
- **Acceptance Criteria**:
  - Function signature: void asl_util_buffer_memset(asl_buffer_s dest, uint8_t value)
  - Fills entire destination buffer with specified byte value
  - Uses asl_buffer_s for consistent buffer handling

#### ASL_REQ_026
- **Requirement ID**: ASL_REQ_026
- **Objective**: Provide buffer memory copy function
- **Requirement**: The utility module SHALL provide asl_util_buffer_memcpy() for copying buffers
- **Pre-Condition**: Valid asl_buffer_s source and destination buffers with allocated memory
- **Input**: asl_buffer_s dest, asl_buffer_s src
- **Output**: Data copied from source buffer to destination buffer
- **Acceptance Criteria**:
  - Function signature: void asl_util_buffer_memcpy(asl_buffer_s dest, asl_buffer_s src)
  - Copies data from source buffer to destination buffer
  - Uses asl_buffer_s for consistent buffer handling

---

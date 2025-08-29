[Logical View](logical_view.md) | [Development View](development_view.md) | [Process View](process_view.md) | [Physical View](physical_view.md) | [Scenarios](scenarios.md)

---

# Process View

## Table of Contents
- [1. Overview](#1-overview)
- [2. Thread Safety Model](#2-thread-safety-model)
- [3. Memory Management](#3-memory-management)
- [4. Control Flow](#4-control-flow)
- [5. Data Flow](#5-data-flow)
- [6. State Management](#6-state-management)
- [7. References](#7-references)

---

## 1. Overview

Runtime behavior of ASL components, focusing on concurrency, memory management, and data flow patterns.

## 2. Thread Safety Model

```mermaid
---
title: ASL Thread Safety Model (A5)
config:
  theme: base
  themeVariables:
    background: "#ffffff"
    primaryColor: "#e6f3ff"
    primaryTextColor: "#000000"
    primaryBorderColor: "#0066cc"
    lineColor: "#000000"
    secondaryColor: "#f0f0f0"
    tertiaryColor: "#cccccc"
    fontFamily: "arial"
    fontSize: "12px"
---
classDiagram
    class asl_mutex_t {
        <<typedef>>
        +lock_f() function_pointer
        +unlock_f() function_pointer
        +context void_pointer
    }
    
    class ProducerThread {
        +acquire_lock()
        +perform_operation()
        +release_lock()
    }
    
    class ConsumerThread {
        +acquire_lock()
        +perform_operation()
        +release_lock()
    }
    
    ProducerThread --> asl_mutex_t : uses
    ConsumerThread --> asl_mutex_t : uses
    
    classDef mutex fill:#e6f3ff,stroke:#0066cc,stroke-width:2px
    classDef thread fill:#f0f0f0,stroke:#666666,stroke-width:1px
    
    class asl_mutex_t:::mutex
    class ProducerThread:::thread
    class ConsumerThread:::thread
```

ASL implements a producer-consumer thread safety model using the `asl_mutex_t` abstraction defined in `asl_types.h`. This model ensures:

1. **Thread Isolation**: Each data structure supports separate producer and consumer threads
2. **Critical Section Protection**: Operations are protected by mutex locks during critical sections
3. **Platform Independence**: The mutex implementation is abstracted through function pointers
4. **Lock Granularity**: Fine-grained locking at the operation level rather than object level

ASL implements thread safety through:
- Mutex abstraction (`asl_mutex_t`)
- Producer-Consumer pattern
- Atomic operations where needed
- Lock-free options for performance

## 3. Memory Management

```mermaid
---
title: ASL Memory Management Flow (A5)
config:
  theme: base
  themeVariables:
    background: "#ffffff"
    primaryColor: "#e6f3ff"
    primaryTextColor: "#000000"
    primaryBorderColor: "#0066cc"
    lineColor: "#000000"
    secondaryColor: "#f0f0f0"
    tertiaryColor: "#cccccc"
    fontFamily: "arial"
    fontSize: "12px"
---
sequenceDiagram
    participant App as Application
    participant ASL as ASL Component
    participant Alloc as Memory Allocator
    
    App->>ASL: Initialize Component
    ASL->>Alloc: Request Memory
    Alloc-->>ASL: Memory Block
    ASL-->>App: Handle
    
    App->>ASL: Use Component
    ASL->>ASL: Manage Memory
    
    App->>ASL: Cleanup
    ASL->>Alloc: Free Memory
    Alloc-->>ASL: Memory Released
    ASL-->>App: Cleanup Complete
```

## 4. Control Flow

Standard operation flow: Parameter validation → Resource check → Lock acquisition → Operation → Lock release → Result return.

## 5. Data Flow

```mermaid
---
title: ASL Data Structure Flow Patterns (A5)
config:
  theme: base
  themeVariables:
    background: "#ffffff"
    primaryColor: "#e6f3ff"
    primaryTextColor: "#000000"
    primaryBorderColor: "#0066cc"
    lineColor: "#000000"
    secondaryColor: "#f0f0f0"
    tertiaryColor: "#cccccc"
    fontFamily: "arial"
    fontSize: "12px"
---
flowchart LR
    subgraph CBUF[Circular Buffer]
        CB_W[Write Ptr] --> CB_B[Buffer]
        CB_B --> CB_R[Read Ptr]
    end
    
    subgraph FIFO[FIFO Queue]
        F_H[Head] --> F_B[Buffer]
        F_B --> F_T[Tail]
    end
    
    subgraph LIFO[LIFO Stack]
        L_T[Top] --> L_B[Buffer]
    end
    
    classDef buf fill:#e6f3ff,stroke:#0066cc,stroke-width:2px
    classDef ptr fill:#ffe6e6,stroke:#cc0000,stroke-width:2px
    class CB_B,F_B,L_B buf
    class CB_W,CB_R,F_H,F_T,L_T ptr
```

## 6. State Management

```mermaid
---
title: ASL Component Lifecycle (A5)
config:
  theme: base
  themeVariables:
    background: "#ffffff"
    primaryColor: "#e6f3ff"
    primaryTextColor: "#000000"
    primaryBorderColor: "#0066cc"
    lineColor: "#000000"
    secondaryColor: "#f0f0f0"
    tertiaryColor: "#cccccc"
    fontFamily: "arial"
    fontSize: "12px"
---
stateDiagram-v2
    [*] --> Uninitialized
    Uninitialized --> Initialized : Init()
    Initialized --> Ready : Configure()
    Ready --> Active : Start Operation
    Active --> Ready : Operation Complete
    Active --> Error : Operation Failed
    Error --> Ready : Reset
    Ready --> Initialized : Reset
    Initialized --> Uninitialized : Cleanup()
    Uninitialized --> [*]
```

Key States:
1. **Uninitialized**: Initial state
2. **Initialized**: Basic setup done
3. **Ready**: Configured for use
4. **Active**: Processing data
5. **Error**: Fault condition

## 7. References

### Implementation Files
- ASL Thread Safety: `asl_mutex_t` in `asl_types.h`
- Memory Management: `asl_allocator_t` in `asl_types.h`
- Data Structures: Implementation files in `library/`

### Related Views
- [Logical View](logical_view.md) - Component interfaces and APIs
- [Development View](development_view.md) - Code organization
- [Scenarios](scenarios.md) - Concrete threading examples

### Documentation Hub
- [Architecture Home](README.md) - Documentation structure and navigation

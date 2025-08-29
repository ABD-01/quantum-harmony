# Logical View: Interface Types

**Parent:** [Types System Hub](logical_view_types.md) | [Logical View](logical_view.md) | [Architecture Home](README.md)

## Table of Contents
- [1. Overview](#1-overview)
- [2. Interface Architecture](#2-interface-architecture)
- [3. Function Pointer Patterns](#3-function-pointer-patterns)
- [4. Public vs Private Interfaces](#4-public-vs-private-interfaces)
- [5. Pattern Classification](#5-pattern-classification)
- [6. Usage Examples](#6-usage-examples)
- [7. References](#7-references)

---

## 1. Overview

The ASL Interface Types module defines comprehensive function pointer patterns used throughout the library. These patterns provide type-safe callback mechanisms and enable flexible, pluggable component architectures.

**Key Features**:
- Systematic function pointer naming convention
- Complete coverage of parameter/return type combinations
- Public/private interface separation
- Type-safe callback mechanisms

---

## 2. Interface Architecture

```mermaid
---
title: ASL Interface Function Types (A5)
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
    class asl_iface_types {
        <<public>>
    }
    
    class asl_iface_priv_types {
        <<private>>
        +asl_iface_v_v_t
        +asl_iface_v_pv_t
        +asl_iface_pv_s_t
        +asl_iface_s_pv_t
    }
    
    class asl_iface_composite_types {
        <<public>>
    }
    
    class asl_iface_composite_priv_types {
        <<private>>
        +asl_iface_composite_rx_tx_t
    }
    
    asl_iface_types --> asl_iface_priv_types : includes
    asl_iface_composite_types --> asl_iface_composite_priv_types : includes
    
    classDef public fill:#ffffff,stroke:#000000,stroke-width:3px,color:#000000
    classDef private fill:#f0f0f0,stroke:#000000,stroke-width:3px,color:#000000
    
    class asl_iface_types:::public
    class asl_iface_priv_types:::private
    class asl_iface_composite_types:::public
    class asl_iface_composite_priv_types:::private
```

---

## 3. Function Pointer Patterns

### 3.1 Naming Convention

ASL follows systematic naming: `asl_iface_[return]_[param]_t`

**Type Codes**:
- `v` = void, `pv` = pvoid, `s` = size_t, `ps` = psize_t
- `u8` = uint8_t, `pu8` = puint8_t, `u16` = uint16_t, `pu16` = puint16_t
- `u32` = uint32_t, `pu32` = puint32_t, `u64` = uint64_t, `pu64` = puint64_t

### 3.2 Basic Function Patterns

```mermaid
---
title: ASL Basic Function Pointer Patterns (A5)
config:
  theme: base
  themeVariables:
    background: "#ffffff"
    primaryColor: "#e6ffe6"
    primaryTextColor: "#000000"
    primaryBorderColor: "#006600"
    lineColor: "#000000"
    secondaryColor: "#f0f0f0"
    tertiaryColor: "#cccccc"
---
classDiagram
    class Void_Return_Functions {
        <<function_pointers>>
        +asl_iface_v_v_t void(*)(void)
        +asl_iface_v_pv_t void(*)(pvoid)
        +asl_iface_v_s_t void(*)(size_t)
        +asl_iface_v_u8_t void(*)(uint8_t)
        +asl_iface_v_u16_t void(*)(uint16_t)
        +asl_iface_v_u32_t void(*)(uint32_t)
        +asl_iface_v_u64_t void(*)(uint64_t)
    }
    
    class Pointer_Return_Functions {
        <<function_pointers>>
        +asl_iface_pv_v_t pvoid(*)(void)
        +asl_iface_pv_pv_t pvoid(*)(pvoid)
        +asl_iface_pv_s_t pvoid(*)(size_t)
        +asl_iface_pv_u8_t pvoid(*)(uint8_t)
        +asl_iface_pv_u16_t pvoid(*)(uint16_t)
        +asl_iface_pv_u32_t pvoid(*)(uint32_t)
        +asl_iface_pv_u64_t pvoid(*)(uint64_t)
    }
    
    class Size_Return_Functions {
        <<function_pointers>>
        +asl_iface_s_v_t size_t(*)(void)
        +asl_iface_s_pv_t size_t(*)(pvoid)
        +asl_iface_s_s_t size_t(*)(size_t)
        +asl_iface_s_u8_t size_t(*)(uint8_t)
        +asl_iface_s_u16_t size_t(*)(uint16_t)
        +asl_iface_s_u32_t size_t(*)(uint32_t)
        +asl_iface_s_u64_t size_t(*)(uint64_t)
    }
    
    classDef void_funcs fill:#e6f3ff,stroke:#0066cc,stroke-width:2px
    classDef ptr_funcs fill:#fff7e6,stroke:#cc7700,stroke-width:2px
    classDef size_funcs fill:#f0fff0,stroke:#228b22,stroke-width:2px
    
    class Void_Return_Functions:::void_funcs
    class Pointer_Return_Functions:::ptr_funcs
    class Size_Return_Functions:::size_funcs
```

---

## 4. Public vs Private Interfaces

### 4.1 Public Interface Headers

**File**: `asl_iface_types.h`
```c
// Public interface - includes private implementation
#include "asl_iface_priv_types.h"

// This file enables EA UML delegate class usage
// Function pointer prototypes defined in private file
```

**File**: `asl_iface_composite_types.h`
```c
// Public composite interface - includes private implementation
#include "asl_iface_composite_priv_types.h"

// Composite function patterns for complex operations
```

### 4.2 Private Implementation Headers

**File**: `asl_iface_priv_types.h`
- Contains 320+ function pointer typedef definitions
- Systematic coverage of all return/parameter combinations
- Foundation for all interface patterns

**File**: `asl_iface_composite_priv_types.h`
- Specialized function patterns for composite operations
- Serial communication interfaces
- Buffer-based operations

---

## 5. Pattern Classification

**Coverage**: 320+ function pointer patterns for all return/parameter combinations.

**Common Patterns**:
- `asl_iface_v_v_t` - void(*)(void) - Simple callbacks
- `asl_iface_pv_s_t` - pvoid(*)(size_t) - Allocation patterns  
- `asl_iface_s_pv_t` - size_t(*)(pvoid) - Size query patterns
- `asl_iface_v_pv_t` - void(*)(pvoid) - Cleanup patterns

**Usage Examples**:
```c
// Allocator uses interface types
typedef struct asl_allocator_t {
    asl_iface_pv_s_t alloc_f;  // pvoid (*)(size_t)
    asl_iface_v_pv_t free_f;   // void (*)(pvoid)
} asl_allocator_t;

// Mutex uses interface types  
typedef struct asl_mutex_t {
    asl_iface_v_v_t lock_f;    // void (*)(void)
    asl_iface_v_v_t unlock_f;  // void (*)(void)
} asl_mutex_t;
```
---

## 6. Usage Examples

**Callback Registration**:
```c
asl_iface_v_pu8_t callback = my_data_handler;
register_callback(callback);
```

**Object Method Dispatch**:
```c
obj->init_f();  // Call initialization function pointer
obj->cleanup_f();  // Call cleanup function pointer
```

**Polymorphic Interface**:
```c
asl_processor_t* proc = get_processor();
proc->process_f(proc, data);  // Call through interface
```

---

## 7. References

### Implementation Files
- **Headers**: `asl_iface_types.h`, `asl_iface_priv_types.h`
- **Composite**: `asl_iface_composite_types.h`, `asl_iface_composite_priv_types.h`

### Related Documentation
- [Basic Types](logical_view_types_basic.md)
- [Composite Types](logical_view_types_composite.md)
- [Type System Hub](logical_view_types.md)

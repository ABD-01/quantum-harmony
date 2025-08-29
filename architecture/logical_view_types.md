# Logical View: Core Type System

**Parent:** [Logical View](logical_view.md) | [Architecture Home](README.md)

## Table of Contents
- [1. Overview](#1-overview)
- [2. Type System Modules](#2-type-system-modules)
- [3. Navigation](#3-navigation)
- [4. References](#4-references)

---

## 1. Overview

The ASL Core Type System provides a comprehensive foundation for all ASL components through a layered type hierarchy. The system includes basic C99-derived types, platform-independent pointer abstractions, function pointer interfaces, and composite types for complex data structures.

**Design Philosophy**: Type safety through abstraction, consistent pointer usage, and modular interface definitions.

---

## 2. Type System Modules

The ASL Type System is organized into three logical modules for maintainability and A5 printability:

### 2.1 Basic & Pointer Types
**Module**: [Basic & Pointer Types](logical_view_types_basic.md)
- C99-derived basic types
- Pointer type abstractions  
- Constant pointer types
- Type hierarchy foundation

### 2.2 Interface Types
**Module**: [Interface Types](logical_view_types_interface.md)
- Function pointer patterns
- Interface type definitions
- Public vs. private interfaces
- Callback mechanisms

### 2.3 Composite Types
**Module**: [Composite Types](logical_view_types_composite.md)
- Memory management types
- Thread safety types
- Composite data structures
- Type dependencies

---

## 3. Navigation

### Quick Access
- **Basic Types**: [logical_view_types_basic.md](logical_view_types_basic.md)
- **Interface Types**: [logical_view_types_interface.md](logical_view_types_interface.md)  
- **Composite Types**: [logical_view_types_composite.md](logical_view_types_composite.md)

### Related Documentation
- **CBUF Types**: [logical_view_cbuf.md](logical_view_cbuf.md)
- **FIFO Types**: [logical_view_fifo.md](logical_view_fifo.md)
- **LIFO Types**: [logical_view_lifo.md](logical_view_lifo.md)
- **Utility Types**: [logical_view_util.md](logical_view_util.md)

---

## 4. References

### Implementation Files
- **Type Definitions**: `asl_types.h`
- **Pointer Types**: `asl_pointer_types.h`
- **Interface Types**: `asl_iface_types.h`
- **Private Interfaces**: `asl_iface_priv_types.h`
- **Composite Types**: `asl_iface_composite_types.h`
- **Composite Private**: `asl_iface_composite_priv_types.h`

### Architecture Documentation
- **Logical View Overview**: [logical_view.md](logical_view.md)
- **Development View**: [development_view.md](development_view.md)
- **Process View**: [process_view.md](process_view.md)
- **Physical View**: [physical_view.md](physical_view.md)

### Requirements Traceability
- **Functional Requirements**: [../requirements/functional_requirements.md](../requirements/functional_requirements.md)
- **Architecture Requirements**: [../requirements/architecture_requirements.md](../requirements/architecture_requirements.md)

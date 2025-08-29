# Logical View: Composite Types Hub

**Parent:** [Types System Hub](logical_view_types.md) | [Logical View](logical_view.md) | [Architecture Home](README.md)

## Overview

The ASL Composite Types system combines basic types and interfaces to create sophisticated abstractions for memory management, thread synchronization, and advanced operations. This modular documentation provides comprehensive coverage of all composite type implementations.

## Modules

### Memory Management: [Memory Management Types](logical_view_types_memory.md)
Core memory abstractions including buffer and allocator types.

**Key Components**:
- `asl_buffer_s` - Bounded memory with size tracking
- `asl_allocator_t` - Pluggable allocation strategies

### Advanced Types: [Advanced Composite Types](logical_view_types_advanced.md)  
Specialized types for thread safety and complex interfaces.

**Key Components**:
- `asl_mutex_t` - Thread synchronization primitives
- `asl_serial_iface_t` - Serial communication abstractions

## Quick Reference

| Type | Purpose | Module |

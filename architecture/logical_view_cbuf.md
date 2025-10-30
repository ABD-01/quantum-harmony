# Logical View: CBUF (Circular Buffer) Hub

**Parent:** [Logical View](logical_view.md) | [Architecture Home](README.md)

## Overview

The Circular Buffer (CBUF) module provides a thread-safe, generic 8-bit data circular buffer implementation for producer-consumer scenarios. This modular documentation covers all aspects of CBUF design, implementation, and usage.

## Modules

### Overview: [Overview & Structure](logical_view_cbuf_overview.md)
Core concepts, internal structure, and operation flow.

**Key Topics**:
- Buffer structure and memory layout
- Operation state machine
- Key features and use cases

### Thread Safety: [Thread Safety & Error Handling](logical_view_cbuf_threading.md)
Concurrency model and error management.

**Key Topics**:
- Thread safety guarantees
- Error handling strategies
- Recovery mechanisms

## Quick Reference

| Function | Purpose | Module |
|----------|---------|--------|
| `asl_cbuf__init()` | Initialize buffer | [Overview](logical_view_cbuf_overview.md) |
| `asl_cbuf__enqueue()` | Add data | [Overview](logical_view_cbuf_overview.md) |
| `asl_cbuf__dequeue()` | Remove data | [Overview](logical_view_cbuf_overview.md) |
| `asl_cbuf__preview()` | Inspect data | [Overview](logical_view_cbuf_overview.md) |

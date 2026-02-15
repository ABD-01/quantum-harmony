# Logical View: FIFO (First-In-First-Out) Hub

**Parent:** [Logical View](logical_view.md) | [Architecture Home](../index.md)

## Overview

The First-In-First-Out (FIFO) module provides a generic queue data structure with 8-bit element storage, fixed capacity, and efficient producer-consumer operation support. This modular documentation covers all aspects of FIFO design, implementation, and usage.

## Modules

### Overview: [Overview & Structure](logical_view_fifo_overview.md)
Core queue concepts, internal structure, and operation flow.

**Key Topics**:
- Queue structure and memory layout
- Operation state machine
- Queue invariants and constraints

## Quick Reference

| Function | Purpose | Module |
|----------|---------|--------|
| `asl_fifo__reset()` | Clear queue | [Overview](logical_view_fifo_overview.md) |
| `asl_fifo__enqueue()` | Add element | [Overview](logical_view_fifo_overview.md) |


```{toctree}
:hidden:
:maxdepth: 1

logical_view_fifo_overview
```

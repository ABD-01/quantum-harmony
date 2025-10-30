[Logical View](logical_view.md) | [Development View](development_view.md) | [Process View](process_view.md) | [Physical View](physical_view.md) | [Scenarios](scenarios.md)

---

# Development View

## Table of Contents
- [1. Overview](#1-overview)
- [2. Directory Structure](#2-directory-structure)
- [3. Package Organization](#3-package-organization)
- [4. Build System](#4-build-system)
- [5. Platform Support](#5-platform-support)
- [6. Dependencies](#6-dependencies)
- [7. References](#7-references)

---

## 1. Overview

The Development View organizes the ASL codebase into clear layers: type definitions, library implementations, and build system integration. This structure enables clean separation of interfaces and implementations while supporting multiple platforms.

## 2. Directory Structure

```mermaid
---
title: ASL Directory Structure (A5)
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
---
graph TD
    subgraph ASL[ASL Root]
        B[build_all.sh]
        BS[build_module.sh]
        C[clean_all.sh]
        subgraph SRC[asl/]
            D[defs/]
            L[library/]
        end
    end
    style ASL fill:#f5f5f5,stroke:#333,stroke-width:2px
    style SRC fill:#e6f3ff,stroke:#0066cc,stroke-width:2px
    style DOC fill:#e6ffe6,stroke:#006600,stroke-width:2px
    style D fill:#fff7e6,stroke:#cc7700,stroke-width:2px
    style L fill:#ffe6e6,stroke:#cc0000,stroke-width:2px
```

## 3. Package Organization

```mermaid
---
title: ASL Package Dependencies (A5)
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
---
classDiagram
    %% Type System
    class ASL_Types_Package {
        <<package>>
        +asl_types.h
        +asl_pointer_types.h
    }
    class ASL_Interfaces_Package {
        <<package>>
        +asl_iface_types.h
        +asl_iface_priv_types.h
        +asl_iface_composite_types.h
        +asl_iface_composite_priv_types.h
    }
    %% Library Implementation
    class ASL_DataStructures_Package {
        <<package>>
        +asl_cbuf.c/.h
        +asl_fifo.c/.h
        +asl_lifo.c/.h
    }
    class ASL_Utils_Package {
        <<package>>
        +asl_util.c/.h
    }
    ASL_DataStructures_Package --> ASL_Types_Package : uses
    ASL_DataStructures_Package --> ASL_Interfaces_Package : uses
    ASL_Utils_Package --> ASL_Types_Package : uses
    ASL_Utils_Package --> ASL_Interfaces_Package : uses
    classDef pkg fill:#f5f5f5,stroke:#333,stroke-width:2px
    class ASL_Types_Package:::pkg
    class ASL_Interfaces_Package:::pkg
    class ASL_DataStructures_Package:::pkg
    class ASL_Utils_Package:::pkg
```

## 4. Build System

The ASL build system supports multiple platforms through a flexible toolchain configuration:

- **Build Scripts**:
  - `build_all.sh`: Builds for all supported platforms
  - `build_module.sh`: Platform-specific build script
  - `clean_all.sh`: Cleanup script

- **Supported Platforms**:
  - STM32 (ARM)
  - TI MCU
  - NXP MCU
  - Microchip MCU
  - Default (x86_64)

- **Build Process**:
  1. Toolchain setup
  2. Source preparation
  3. Compilation
  4. Library creation
  5. Export & packaging

## 5. Platform Support

```mermaid
---
title: ASL Platform Support Architecture (A5)
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
---
classDiagram
    class ASL_Core {
        <<library>>
        +Data Structures
        +Utilities
    }
    class Platform_ST {
        <<toolchain>>
        +ARM GCC
        +STM32 specifics
    }
    class Platform_TI {
        <<toolchain>>
        +TI ARM LLVM
        +TI MCU specifics
    }
    class Platform_NXP {
        <<toolchain>>
        +ARM GCC
        +NXP specifics
    }
    class Platform_MC {
        <<toolchain>>
        +XC32
        +ATSAM specifics
    }
    ASL_Core <-- Platform_ST : builds for
    ASL_Core <-- Platform_TI : builds for
    ASL_Core <-- Platform_NXP : builds for
    ASL_Core <-- Platform_MC : builds for
    classDef core fill:#e6f3ff,stroke:#0066cc,stroke-width:2px
    classDef plat fill:#ffe6e6,stroke:#cc0000,stroke-width:2px
    class ASL_Core:::core
    class Platform_ST:::plat
    class Platform_TI:::plat
    class Platform_NXP:::plat
    class Platform_MC:::plat
```

## 6. Dependencies

ASL is designed to be self-contained with minimal external dependencies:

- **Required**:
  - Standard C library
  - Platform toolchain
  - Build shell environment

- **Optional**:
  - None currently

## 7. References

### Implementation Files
- Source Tree: `asl/` directory
- Build Scripts: Root directory
- Platform Toolchains: As configured in `build_module.sh`

### Related Views
- [Logical View](logical_view.md) - Component structure and relationships
- [Process View](process_view.md) - Runtime dependencies
- [Physical View](physical_view.md) - Platform-specific builds

### Documentation Hub
- [Architecture Home](README.md) - Documentation structure and navigation

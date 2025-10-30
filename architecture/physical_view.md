[Logical View](logical_view.md) | [Development View](development_view.md) | [Process View](process_view.md) | [Physical View](physical_view.md) | [Scenarios](scenarios.md)

---

# Physical View

## Table of Contents
- [1. Overview](#1-overview)
- [2. Deployment Architecture](#2-deployment-architecture)
- [3. Platform Integration](#3-platform-integration)
- [4. Resource Requirements](#4-resource-requirements)
- [5. Build Artifacts](#5-build-artifacts)
- [6. Platform-Specific Considerations](#6-platform-specific-considerations)
- [7. References](#7-references)

## 1. Overview

ASL deployment across different hardware platforms and integration with various microcontroller architectures.

## 2. Deployment Architecture

```mermaid
---
title: ASL Deployment Architecture (A5)
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
C4Deployment
    title ASL Library Deployment Architecture
    
    Deployment_Node(app_layer, "Application Layer", "User Application Code") {
        Container(user_app, "User Application", "C Application", "Application using ASL")
    }
    
    Deployment_Node(asl_layer, "ASL Library Layer", "Static Library") {
        Container(asl_lib, "ASL Library", "C Static Library", "libasl.a")
    }
    
    Deployment_Node(platform_layer, "Platform Layer", "OS/RTOS Abstraction") {
        Container(platform_abs, "Platform Abstraction", "C Headers", "Platform-specific implementations")
    }
    
    Deployment_Node(hw_layer, "Hardware Layer", "Target Hardware") {
        Container(target_hw, "Target Hardware", "Microcontroller", "ARM Cortex-M, x86, etc.")
    }
    
    Rel(user_app, asl_lib, "links with", "static linking")
    Rel(asl_lib, platform_abs, "abstracts", "function pointers")
    Rel(platform_abs, target_hw, "controls", "direct access")
```

## 3. Platform Integration

```mermaid
---
title: ASL Platform Integration (A5)
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
    class ApplicationCode {
        <<user code>>
        +main()
        +application_logic()
    }
    class ASL_Library {
        <<static library>>
        +Data Structures
        +Memory Management
        +Thread Safety
    }
    class Platform_Layer {
        <<hardware>>
        +CPU Architecture
        +Memory System
        +Peripherals
    }
    ApplicationCode --> ASL_Library : links to
    ASL_Library --> Platform_Layer : compiled for
    
    classDef app fill:#e6ffe6,stroke:#006600,stroke-width:2px
    classDef lib fill:#e6f3ff,stroke:#0066cc,stroke-width:2px
    classDef plat fill:#ffe6e6,stroke:#cc0000,stroke-width:2px
    class ApplicationCode:::app
    class ASL_Library:::lib
    class Platform_Layer:::plat
```

## 4. Resource Requirements

**Memory**: Minimal ROM/RAM footprint, stack-aware design  
**CPU**: Standard ALU operations, optional atomic operations  
**Platform**: Optional mutex support for thread safety

## 5. Build Artifacts

For each supported platform, ASL produces:
- `libasl.a`: Static library
- Public API headers
- Type definitions

## 6. Platform-Specific Considerations

```mermaid
---
title: ASL Platform-Specific Features (A5)
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
    class STM32_Platform {
        <<ARM Cortex-M>>
        +CMSIS Support
        +HAL Integration
        +DMA Capability
    }
    class TI_Platform {
        <<ARM Cortex-M>>
        +TI-RTOS Support
        +XDS Debug
        +DSP Extensions
    }
    class NXP_Platform {
        <<ARM Cortex-M>>
        +MCUXpresso
        +FreeRTOS
        +MPU Support
    }
    class MC_Platform {
        <<MIPS/ARM>>
        +XC32 Support
        +PIC32 Features
        +MPLAB Integration
    }
    
    STM32_Platform --|> ASL : implements
    TI_Platform --|> ASL : implements
    NXP_Platform --|> ASL : implements
    MC_Platform --|> ASL : implements
    
    class ASL {
        <<interface>>
        +Standard Features
        +Memory Management
        +Thread Safety
    }
```

Platform Support: STM32, TI MCU, NXP, Microchip (ARM/MIPS architectures)

## 7. References

- Build System: `build_module.sh`, `build_all.sh`
- Platform Configurations: Toolchain setups in build scripts
- Resource Usage: Implementation files in `library/`

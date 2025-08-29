# ASL (Abstract Software Library) Documentation

## Table of Contents
- [Overview](#overview)
- [Documentation Structure](#documentation-structure)
- [Navigation Guide](#navigation-guide)
- [Diagram Standards](#diagram-standards)
- [Update History](#update-history)

---

**Note:** All diagrams in this documentation use the UML 2.5 standard and are rendered in Mermaid syntax. This is implied throughout and not repeated in each view.

---

## Overview

This documentation provides a comprehensive architectural and design reference for the Abstract Software Library (ASL) component. It is organized according to Kruchten's 4+1 architectural views, with each view documented in a dedicated Markdown file and illustrated using Mermaid diagrams. The documentation is specification-driven, code-accurate, and suitable for both maintainers and integrators.

## Diagram Standards

- All diagrams are designed to fit on A5 paper (148mm × 210mm)
- UML 2.5 notation is used consistently across all diagrams
- Color schemes maintain high contrast for readability
- Consistent styling is applied across similar diagram types

## Documentation Structure

- [Logical View](logical_view.md):
  - Describes the logical structure, main modules, APIs, and data types of ASL.
  - Main architectural overview with links to detailed module documentation:
    - [Core Types](logical_view_types.md) - Type system and interfaces
    - [CBUF Module](logical_view_cbuf.md) - Circular Buffer implementation
    - [FIFO Module](logical_view_fifo.md) - First-In-First-Out implementation
    - [LIFO Module](logical_view_lifo.md) - Last-In-First-Out implementation
    - [Utilities](logical_view_util.md) - Common utility functions
- [Development View](development_view.md):
  - Details the codebase organization, module boundaries, and build process.
  - Includes component and package diagrams.
- [Process View](process_view.md):
  - Explains runtime behavior, concurrency, and control/data flow between modules.
  - Includes activity and flow diagrams.
- [Physical View](physical_view.md):
  - Shows the deployment and mapping of ASL components across different platforms.
  - Includes deployment diagrams and platform mapping.
- [Scenarios View](scenarios.md):
  - Illustrates typical usage flows and interactions with the ASL components.
  - Includes sequence diagrams for key scenarios.

---

For detailed information, refer to each view using the links above. All diagrams are printable on A5 paper and are designed for clarity and maintainability.

## Navigation Guide

To navigate this documentation effectively:

1. **Start with the Logical View**: Begin with the main [Logical View](logical_view.md) to understand the system's key abstractions
2. **Explore Component Details**: Dive into specific components through the detailed module documentation
3. **Understand Development Structure**: Check the [Development View](development_view.md) to see code organization
4. **Review Runtime Behavior**: Examine the [Process View](process_view.md) for thread safety and concurrency patterns
5. **Examine Deployment**: See the [Physical View](physical_view.md) for platform integration details
6. **Walk Through Scenarios**: Review the [Scenarios](scenarios.md) for practical usage examples

## Update History

| Date       | Version | Description                                   |
|------------|---------|-----------------------------------------------|
| 2025-08-11 | 1.1.0   | Updated thread safety documentation           |
| 2025-08-11 | 1.1.0   | Improved core type system documentation       |
| 2025-08-11 | 1.1.0   | Enhanced navigation between architectural views |
| 2025-04-03 | 1.0.0   | Initial documentation creation                |

---

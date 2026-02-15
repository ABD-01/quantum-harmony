# Welcome to ATP::ASL's documentation!

```{toctree}
---
maxdepth: 2
caption: API References
hidden:
---

File List <api/filelist>
Struct List <api/structlist>

```

```{toctree}
---
maxdepth: 2
caption: Contents
hidden:
---

Process View <architecture/process_view>
Logical View <architecture/logical_view>
Development View <architecture/development_view>
Physical View <architecture/physical_view>
Scenarios <architecture/scenarios>

```

## Overview

This documentation provides a comprehensive architectural and design reference for the Abstract Software Library (ASL) component. It is organized according to Kruchten's 4+1 architectural views, with each view documented in a dedicated Markdown file and illustrated using Mermaid diagrams. The documentation is specification-driven, code-accurate, and suitable for both maintainers and integrators.

## Diagram Standards

- All diagrams are designed to fit on A5 paper (148mm × 210mm)
- UML 2.5 notation is used consistently across all diagrams
- Color schemes maintain high contrast for readability
- Consistent styling is applied across similar diagram types

## Documentation Structure

- [Logical View](architecture/logical_view.md):
  - Describes the logical structure, main modules, APIs, and data types of ASL.
  - Main architectural overview with links to detailed module documentation:
    - [Core Types](architecture/logical_view_types.md) - Type system and interfaces
    - [CBUF Module](architecture/logical_view_cbuf.md) - Circular Buffer implementation
    - [FIFO Module](architecture/logical_view_fifo.md) - First-In-First-Out implementation
    - [LIFO Module](architecture/logical_view_lifo.md) - Last-In-First-Out implementation
    - [Utilities](architecture/logical_view_util.md) - Common utility functions
- [Development View](architecture/development_view.md):
  - Details the codebase organization, module boundaries, and build process.
  - Includes component and package diagrams.
- [Process View](architecture/process_view.md):
  - Explains runtime behavior, concurrency, and control/data flow between modules.
  - Includes activity and flow diagrams.
- [Physical View](architecture/physical_view.md):
  - Shows the deployment and mapping of ASL components across different platforms.
  - Includes deployment diagrams and platform mapping.
- [Scenarios View](architecture/scenarios.md):
  - Illustrates typical usage flows and interactions with the ASL components.
  - Includes sequence diagrams for key scenarios.

---

For detailed information, refer to each view using the links above. All diagrams are printable on A5 paper and are designed for clarity and maintainability.

## Navigation Guide

To navigate this documentation effectively:

1. **Start with the Logical View**: Begin with the main [Logical View](architecture/logical_view.md) to understand the system's key abstractions
2. **Explore Component Details**: Dive into specific components through the detailed module documentation
3. **Understand Development Structure**: Check the [Development View](architecture/development_view.md) to see code organization
4. **Review Runtime Behavior**: Examine the [Process View](architecture/process_view.md) for thread safety and concurrency patterns
5. **Examine Deployment**: See the [Physical View](architecture/physical_view.md) for platform integration details
6. **Walk Through Scenarios**: Review the [Scenarios](architecture/scenarios.md) for practical usage examples

# Indices and tables

* {ref}`genindex`
* {ref}`modindex`

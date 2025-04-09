# ZSweep Code Guidelines

## Build Commands
- Build: `make` (or `make -f Makefile.osx` on macOS)
- Clean: `make clean`
- Run: `./demo`
- Debug: Use OpenGL debug tools via keyboard shortcuts (see zs_ZSweepDemo.cpp)

## Code Style Guidelines
- **Headers**:
  - Use modern C++ headers: `<iostream>`, `<fstream>`, `<vector>` (not `.h` versions)
  - Include `<stdint.h>` when dealing with pointer conversions (use `intptr_t`)

- **Naming**:
  - Files prefixed with `zs_` (e.g., `zs_scene.cpp`)
  - Header files use `.hh` extension
  - Classes use PascalCase (e.g., `Point`, `Cell`)
  - Member variables prefixed with underscore (e.g., `_coord`, `_vertexIndices`)
  - Constants and macros use UPPER_SNAKE_CASE (e.g., `MARK_BOUNDARY`)

- **Memory Management**:
  - Use appropriate delete operators: `delete[]` for arrays
  - Be careful with pointer-to-int conversions, use `intptr_t` when needed

- **Documentation & Error Handling**:
  - Header files include descriptive comment blocks with author and date
  - Use debug macros from `zs_debug.hh` and assertions for error checking

This is a C++ OpenGL graphics rendering project implementing the Z-Sweep algorithm for 3D visualization.
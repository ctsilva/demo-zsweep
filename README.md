# ZSweep Demo

A C++ OpenGL graphics rendering project implementing the Z-Sweep algorithm for 3D visualization and volumetric rendering.

## Overview

ZSweep is a volume rendering technique that uses depth-sorting and OpenGL for efficient 3D visualization. This project provides a complete implementation with an interactive demo application for exploring 3D datasets.

This implementation is based on the research paper:
**"ZSWEEP: an efficient and exact projection algorithm for unstructured volume rendering"**  
*Ricardo C. Farias, Joseph S. B. Mitchell, Cláudio T. Silva*  
VVS 2000: 91-99

## Features

- **Z-Sweep Algorithm**: Efficient volume rendering with depth-sorting
- **OpenGL Rendering**: Hardware-accelerated 3D graphics
- **Interactive Demo**: Real-time manipulation and visualization
- **Cross-Platform**: Supports Linux/Unix and macOS
- **Multiple Entry Points**: Various main functions for different use cases

## Requirements

### Dependencies
- **OpenGL**: Graphics rendering
- **GLUT**: Windowing and input handling
- **X11** (Linux/Unix): Window system
- **Carbon Framework** (macOS): System integration

### Build Tools
- **CMake**: Modern cross-platform build system (recommended)
- **Make**: Alternative traditional build system
- **GCC/Clang**: C++ compiler with C++11 support

## Building

### Using CMake (Recommended)
```bash
mkdir build && cd build
cmake ..
make

# Or use cmake --build for cross-platform compatibility
cmake --build .
```

### Using Make (Alternative)
```bash
# Linux/Unix
make

# macOS
make -f Makefile.osx

# Clean build artifacts
make clean
```

## Usage

### Running the Demo
```bash
./demo
```

### Keyboard Controls
The demo supports various keyboard shortcuts for debugging and interaction (see `zs_ZSweepDemo.cpp` for complete list).

## Project Structure

### Core Components
- **`zs_scene.*`**: Scene management and 3D objects
- **`zs_render.*`**: Main rendering pipeline
- **`zs_OpenGL.*`**: OpenGL interface and utilities
- **`zs_basic.*`**: Fundamental data structures and utilities

### Data Structures
- **`zs_cell.*`**: Spatial subdivision cells
- **`zs_heap.*`**: Priority queue for depth sorting
- **`zs_hash.*`**: Hash table implementations
- **`zs_screenLists.*`**: Screen-space data structures

### Input/Output
- **`zs_readToff.*`**: File format parsing
- **`zs_boundary.*`**: Boundary condition handling

### Demo Applications
- **`zs_ZSweepDemo.*`**: Main interactive demo
- **`zs_mainOriginal.*`**: Original implementation entry point
- **`zs_mainAPI.*`**: API-based interface
- **`zs_mainManual.*`**: Manual configuration interface

## File Formats

The project supports various 3D data formats including:
- **`.toff`**: Tetrahedral mesh format
- **`.off`**: Object file format
- **`.tf`**: Transfer function files
- **`.ppm`**: Image output format

## Development

### Code Style
- Files prefixed with `zs_`
- Header files use `.hh` extension
- Modern C++ headers (no `.h` versions)
- PascalCase for classes, underscore prefix for members
- Comprehensive error checking and debug macros

### Debug Features
- Debug macros in `zs_debug.hh`
- OpenGL debugging tools
- Assertion-based error checking
- Performance profiling support

## Authors

- **Ricardo C. Farias** - rfarias@ams.sunysb.edu
- **Joseph S. B. Mitchell**
- **Cláudio T. Silva**

Applied Mathematics and Statistics  
State University of New York at Stony Brook

## Citation

If you use this code in your research, please cite the original paper:

```bibtex
@inproceedings{farias2000zsweep,
  title={ZSWEEP: an efficient and exact projection algorithm for unstructured volume rendering},
  author={Farias, Ricardo C. and Mitchell, Joseph S. B. and Silva, Cl{\'a}udio T.},
  booktitle={VVS 2000},
  pages={91--99},
  year={2000}
}
```

## License

This project is part of academic research at SUNY Stony Brook. Please refer to the original authors for licensing information.

## Contributing

When contributing to this project:
1. Follow the established code style guidelines
2. Use appropriate debug macros for error checking
3. Test on both Linux and macOS platforms
4. Document any new features or significant changes

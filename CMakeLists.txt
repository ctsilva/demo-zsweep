cmake_minimum_required(VERSION 3.10)
project(zsweep)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    zs_general.cpp
    zs_basic.cpp
    zs_cell.cpp
    zs_heap.cpp
    zs_screenLists.cpp
    zs_hash.cpp
    zs_debugFuncs.cpp
    zs_readToff.cpp
    zs_boundary.cpp
    zs_sparse.cpp
    zs_scene.cpp
    zs_render.cpp
    zs_renderPAR.cpp
    zs_fakeSMP.cpp
    zs_OpenGL.cpp
    zs_mainOriginal.cpp
    zs_ZSweepDemo.cpp
)

# Executable
add_executable(demo ${SOURCES})

# Include directories
target_include_directories(demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Platform-specific settings
if(APPLE)
    # macOS specific settings
    target_compile_definitions(demo PRIVATE _BSD_SOURCE LINUX)
    target_compile_options(demo PRIVATE -Wall -Wno-deprecated-declarations)
    
    # Find required packages
    find_package(OpenGL REQUIRED)
    
    # Link libraries
    target_link_libraries(demo PRIVATE
        "-framework GLUT"
        "-framework OpenGL"
        "-framework Carbon"
        m
        objc
    )
else()
    # Linux/Unix specific settings
    target_compile_definitions(demo PRIVATE _BSD_SOURCE LINUX)
    target_compile_options(demo PRIVATE -Wall)
    
    # Find required packages
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    find_package(X11 REQUIRED)
    
    # Include directories
    target_include_directories(demo PRIVATE
        /usr/freeware/include
    )
    
    # Link directories
    target_link_directories(demo PRIVATE
        /usr/freeware/lib32
        /usr/X11R6/lib
    )
    
    # Link libraries
    target_link_libraries(demo PRIVATE
        ${GLUT_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${X11_LIBRARIES}
        Xext
        Xmu
        Xi
        m
    )
endif()
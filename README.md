# VoxelEngine

A lightweight, cross-platform **voxel engine** written in C using **OpenGL** and **GLFW**. Designed for experimentation and open-source contributions.  

---

## Features

- Cross-platform: Windows, macOS, Linux  
- OpenGL 3.3 Core Profile (GPU accelerated)  
- 60 FPS fixed frame loop with FPS counter  
- Modular, minimal codebase ready for voxel rendering  

---

## Getting Started

### Requirements

- C compiler (GCC, Clang, or Visual Studio for Windows)  
- [GLFW 64-bit binaries](https://www.glfw.org/download.html)  
- [GLAD OpenGL loader](https://glad.dav1d.de/) (C/C++, OpenGL 3.3 Core)  
- OpenGL 3.3 compatible GPU drivers  

---

### Setup

1. Download and unzip **GLFW**.  
2. Place `glfw3.dll` in the same directory as your executable.  
3. Include GLFW and GLAD headers in your project:  

```c
#include <glad/glad.h>
#include <GLFW/glfw3.h>

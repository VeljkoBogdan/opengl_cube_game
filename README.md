### Voxel engine made using C++ and OpenGL

Voxel engine made using C++ and OpenGL with tools (Glad, GLFW) to learn graphics rendering and pipeline basics, as well as 3D optimizations.

* C++ version: `17`
* OpenGL version: `4.3.0`
* GLFW: `3.4`
* FastNoise2: `1.1.1`
* CMake: `3.30.5`

## Features:
* Infinite procedural 3D world in all directions (x, y, z) using FastNoise2
* Surface, caves, caverns and flying islands
* Batched 16x16x16 chunk rendering
* Directional diffuse lighting and FXAA shaders
* Concurrent world generation and remeshing
* Placing and destroying blocks using right and left click respectivelly

<img width="1920" height="1080" alt="cubegame" src="https://github.com/user-attachments/assets/fa1754e2-fd94-48e0-aa82-ca4c11e985f5" />

## More info
FastNoise2 allows for world generation using SIMD instructions, which speeds up world gen to up to 8x that of the OpenSimplex generator.

The renderer has about 2000 FPS on an RTX 3060 12GB, and about 60-90 FPS on the Vega 8 iGPU with the render distance of 32 (meaning 32768 chunks are being rendered per frame!). This is mostly possible by batch rendering, work from the CPU is offloaded onto the GPU and rendered using the Indirect Buffer functionality provided in OpenGL 4.3, so the chunk rendering is mostly bottlenecked by the GPU.

Because the indirect buffer requires OpenGL 4.3, this program is naturally not supported by MacOS, which limits OpenGL to 4.1. This is a willing sacrifice as the game would be unplayable without the optimizations.

### Voxel engine made using C++ and OpenGL

Voxel engine made using C++ and OpenGL with tools (Glad, GLFW) to learn graphics rendering and pipeline basics, as well as 3D optimizations.

<img width="1274" height="715" alt="Screenshot 2026-11-17 114702" src="https://github.com/user-attachments/assets/5ed0421d-21f3-437e-acc6-602cffd3c9a2" />

## Features:
* Infinite procedural 3D world in all directions (x, y, z) using FastNoise2
* Surface, caves, caverns and flying islands
* Batched 16x16x16 chunk rendering
* Directional diffuse lighting, Ambient Occlusion, and Fast Approximate Anti-Aliasing
* Day-night cycle
* Concurrent world generation and remeshing
* Placing and destroying blocks using right and left click respectivelly

<img width="1274" height="711" alt="Screenshot 2026-11-17 114945" src="https://github.com/user-attachments/assets/34c484fc-0c76-4b66-866c-64020a11fcc8" />

## More info
FastNoise2 allows for world generation using SIMD instructions, which speeds up world gen to up to 8x that of the OpenSimplex generator.

The renderer has about 2000 FPS on an RTX 3060 12GB, and about 60-90 FPS on the Vega 8 iGPU with the render distance of 32 (meaning 32768 chunks are being rendered per frame!). This is mostly possible by batch rendering, work from the CPU is offloaded onto the GPU and rendered using the Indirect Buffer functionality provided in OpenGL 4.3, so the chunk rendering is mostly bottlenecked by the GPU.

Because the indirect buffer requires OpenGL 4.3, this program is naturally not supported by MacOS, which limits OpenGL to 4.1. This is a willing sacrifice as the game would be unplayable without the optimizations.

<img width="1920" height="1080" alt="cubegame" src="https://github.com/user-attachments/assets/f48f1ddb-b70e-4f0b-a9ee-823f99b7c7c6" />

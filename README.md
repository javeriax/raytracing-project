# Raytracing Gas Station Project

## Overview
This project is a C++ raytracer (built from our homework 5 as the base) that renders a detailed gas station scene with:
- Physically-based materials (Phong, Matte, Reflective, etc.) 
- Area and point lights, including neon and emissive surfaces
- OBJ/MTL geometry loading (cars, bikes, neon signs, etc.)
- Acceleration structure (BVH) for fast ray intersection
- Configurable camera and viewplane
- Bloom for gaussian blur in neon lights (Post Processing)

## How to Build

### Prerequisites
- C++17 compatible compiler (tested with g++)
- All source files and scene assets in the project directory

### Build Command
```
g++ -std=c++17 -O2 build/buildGasStationScene.cpp raytracer.cpp utilities/*.cpp geometry/*.cpp cameras/*.cpp samplers/*.cpp brdf/*.cpp materials/*.cpp lights/*.cpp tracers/*.cpp acceleration/*.cpp image/*.cpp world/*.cpp -I. -o buildGasStationScene.exe
```

## How to Run

### With Acceleration Structure (BVH)
By default, the scene uses a BVH acceleration structure for fast rendering. Just run:
```
./buildGasStationScene.exe
```

### Without Acceleration Structure
To disable the acceleration structure, in raytracer.cpp use: #define USE_ACCELERATION false 


## features
- **Neon Lights:** Emissive materials and point lights simulate neon effects on signs and pillars.
- **OBJ/MTL Loading:** Loads geometry and materials for all scene objects, including custom neon sign meshes.
- **Physically-Based Materials:** Supports diffuse, glossy, specular, and reflective surfaces.
- **Lighting:** Multiple point and directional lights, including canopy, fill, and neon lights.
- **Acceleration Structure:** BVH for efficient ray tracing (can be disabled by setting Flag to FALSE in raytracer.cpp)
- **Post-Processing:** Optional bloom for glowing effects.

## Customization
- **Camera Angles:** Change the camera position in `buildGasStationScene.cpp` for different perspectives.
- **Resolution:** Adjust `vplane.hres` and `vplane.vres` for output image size.
- **Acceleration Structure:** Can be disabled by setting Flag to FALSE in raytracer.cpp
---
For any issues or questions, check the code comments or contact the author.
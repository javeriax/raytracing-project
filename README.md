# Raytracing Gas Station Project

## Overview
This project is a C++ raytracer that renders a detailed gas station scene with:
- Physically-based materials (Phong, Matte, Reflective, etc.)
- Area and point lights, including neon and emissive surfaces
- OBJ/MTL geometry loading (cars, bikes, neon signs, etc.)
- Acceleration structure (BVH) for fast ray intersection
- Configurable camera and viewplane
- Optional bloom and post-processing

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
To disable the acceleration structure, comment out or remove the following line in `buildGasStationScene.cpp`:
```cpp
set_acceleration(new BVH(this));
```
Or set `acceleration_ptr = nullptr;` after building the scene. Then recompile and run as above.

## Camera Setup
- The camera is set in `buildGasStationScene.cpp` with:
	```cpp
	set_camera(new Perspective(x, y, z));
	```
- Default: `(1.26, -5.84, 6.61)` (front, slightly right and above)

### To view from the left (see the left side of the signboard):
Use a camera position like:
```
set_camera(new Perspective(-3.0f, -5.0f, 7.0f));
```
This will move the camera to the left of the scene, giving a clear view of the left side and neon signs.

## Scene Features
- **Neon Lights:** Emissive materials and point lights simulate neon effects on signs and pillars.
- **OBJ/MTL Loading:** Loads geometry and materials for all scene objects, including custom neon sign meshes.
- **Physically-Based Materials:** Supports diffuse, glossy, specular, and reflective surfaces.
- **Lighting:** Multiple point and directional lights, including canopy, fill, and neon lights.
- **Acceleration Structure:** BVH for efficient ray tracing (can be disabled for debugging).
- **Post-Processing:** Optional bloom for glowing effects.

## Customization
- **Camera Angles:** Change the camera position in `buildGasStationScene.cpp` for different perspectives.
- **Resolution:** Adjust `vplane.hres` and `vplane.vres` for output image size.
- **Background Color:** Set `bg_color` in the build function.

---
For any issues or questions, check the code comments or contact the author.
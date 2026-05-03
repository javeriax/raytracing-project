// #pragma once

// #include <string>

// class World;
// class Material;

// // loads a .obj file and adds all triangles to the world
// // supports: vertices (v), faces (f)
// // face format: f v1 v2 v3 or f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
// // quad faces (4 vertices) are split into 2 triangles automatically
// void load_obj(const std::string& filename, World& world, Material* mat);


#pragma once

#include <string>
#include "../utilities/Point3D.hpp"

class World;
class Material;

// transform applied to every vertex when loading
// scale first, then translate
// allows positioning and sizing models in the scene
struct OBJTransform {
    float scale;      // uniform scale factor
    float tx, ty, tz; // translation after scaling

    OBJTransform() : scale(1.0f), tx(0), ty(0), tz(0) {}
    OBJTransform(float s, float x, float y, float z)
        : scale(s), tx(x), ty(y), tz(z) {}

    // apply transform to a vertex
    Point3D apply(const Point3D& p) const {
        return Point3D(p.x * scale + tx,
                       p.y * scale + ty,
                       p.z * scale + tz);
    }
};

// loads a .obj file and adds all triangles to the world
// supports v, f lines - handles quads and ngons via fan triangulation
// face format: f v or f v/vt or f v/vt/vn or f v//vn
void load_obj(const std::string& filename,
              World& world,
              Material* mat,
              const OBJTransform& transform = OBJTransform());

#include <map>

// loads a .obj file with per-material support via usemtl lines
// materials maps material names (from usemtl) to Material pointers
// faces with no usemtl or unknown names use default_mat
void load_obj_mtl(const std::string& filename,
                  World& world,
                  const std::map<std::string, Material*>& materials,
                  Material* default_mat,
                  const OBJTransform& transform = OBJTransform());  
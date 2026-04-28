#pragma once
#include <cstdlib>
#include "Constants.hpp"
#include "Point3D.hpp"
#include "Ray.hpp"
#include "Vector3D.hpp"
class Material;
class World;
class ShadeInfo {
public:
  bool hit;
  Material *material_ptr;
  Point3D hit_point;
  Vector3D normal;
  Ray ray;
  int depth;
  float t;
  const World *w;
  ShadeInfo(const World &wr);
  ShadeInfo(const ShadeInfo &sr) = default;
  ~ShadeInfo() = default;
};

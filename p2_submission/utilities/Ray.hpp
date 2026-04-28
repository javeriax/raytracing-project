#pragma once
#include "Point3D.hpp"
#include "Vector3D.hpp"
class Ray {
public:
  Point3D o; Vector3D d; float w;
  Ray(); Ray(const Point3D &origin, const Vector3D &dir);
  Ray(const Ray &ray) = default;
  Ray &operator=(const Ray &rhs) = default;
  ~Ray() = default;
  std::string to_string() const;
};

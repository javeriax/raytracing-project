#pragma once
#include "Geometry.hpp"
#include "../utilities/Point3D.hpp"
class Sphere : public Geometry {
protected:
  Point3D c; float r;
public:
  Sphere(); Sphere(const Point3D &center, float radius);
  Sphere(const Sphere &object); Sphere &operator=(const Sphere &rhs);
  virtual ~Sphere() = default;
  std::string to_string() const override;
  virtual bool hit(const Ray &ray, float &t, ShadeInfo &s) const override;
  virtual BBox getBBox() const override;
};

#pragma once
#include "Geometry.hpp"
#include "../utilities/Point3D.hpp"
class Triangle : public Geometry {
protected:
  Point3D v0, v1, v2;
public:
  Triangle(); Triangle(const Point3D &, const Point3D &, const Point3D &);
  Triangle(const Triangle &object); Triangle &operator=(const Triangle &rhs);
  virtual ~Triangle() = default;
  std::string to_string() const override;
  virtual bool hit(const Ray &ray, float &t, ShadeInfo &s) const override;
  virtual BBox getBBox() const override;
};

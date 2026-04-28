#pragma once
#include "Geometry.hpp"
#include "../utilities/Point3D.hpp"
#include "../utilities/Vector3D.hpp"
class Plane : public Geometry {
protected:
  Point3D a; Vector3D n;
public:
  Plane(); Plane(const Point3D &pt, const Vector3D &n);
  Plane(const Plane &object); Plane &operator=(const Plane &rhs);
  virtual ~Plane() = default;
  virtual std::string to_string() const override;
  virtual bool hit(const Ray &ray, float &t, ShadeInfo &s) const override;
  virtual BBox getBBox() const override;
};

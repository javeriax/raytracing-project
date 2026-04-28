#pragma once
#include "Point3D.hpp"
class Geometry;
class Ray;
class BBox {
public:
  Point3D pmin, pmax;
  BBox() = default;
  BBox(const Point3D &min, const Point3D &max);
  BBox(const BBox &b) = default;
  BBox &operator=(const BBox &rhs) = default;
  ~BBox() = default;
  std::string to_string() const;
  bool hit(const Ray &ray, float &t_enter, float &t_exit) const;
  void extend(Geometry *g);
  void extend(const BBox &b);
  bool contains(const Point3D &p);
  bool overlaps(Geometry *g);
  bool overlaps(const BBox &b);
};

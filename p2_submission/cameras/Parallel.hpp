#pragma once
#include "Camera.hpp"
#include "../utilities/Vector3D.hpp"
class Parallel : public Camera {
protected:
  Vector3D dir;
public:
  Parallel(); Parallel(float c); Parallel(float x, float y, float z); Parallel(const Vector3D &d);
  Parallel(const Parallel &camera); Parallel &operator=(const Parallel &other);
  virtual ~Parallel() = default;
  virtual Vector3D get_direction(const Point3D &p) const;
};

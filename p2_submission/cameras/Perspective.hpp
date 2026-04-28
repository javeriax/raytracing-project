#pragma once
#include "Camera.hpp"
#include "../utilities/Point3D.hpp"
class Vector3D;
class Perspective : public Camera {
protected:
  Point3D pos;
public:
  Perspective(); Perspective(float c); Perspective(float x, float y, float z); Perspective(const Point3D &pt);
  Perspective(const Perspective &camera); Perspective &operator=(const Perspective &other);
  virtual ~Perspective() = default;
  virtual Vector3D get_direction(const Point3D &p) const;
};

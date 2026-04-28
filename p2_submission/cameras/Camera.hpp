#pragma once
class Point3D; class Vector3D;
class Camera {
public:
  Camera() = default;
  Camera(const Camera &camera) = default;
  Camera &operator=(const Camera &other) = default;
  virtual ~Camera() = default;
  virtual Vector3D get_direction(const Point3D &p) const = 0;
};

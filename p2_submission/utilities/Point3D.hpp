#pragma once
#include <string>
class Vector3D;
class Point3D {
public:
  float x, y, z;
  Point3D(); Point3D(float c); Point3D(float x, float y, float z);
  ~Point3D() = default;
  Point3D(const Point3D &p) = default;
  Point3D &operator=(const Point3D &p) = default;
  std::string to_string() const;
  Point3D operator-() const;
  Vector3D operator-(const Point3D &p) const;
  Point3D operator+(const Vector3D &v) const;
  Point3D operator-(const Vector3D &v) const;
  Point3D operator*(const float s) const;
  float d_squared(const Point3D &p) const;
  float distance(const Point3D &p) const;
};
Point3D operator*(const float a, const Point3D &pt);
Point3D min(const Point3D &a, const Point3D &b);
Point3D max(const Point3D &a, const Point3D &b);

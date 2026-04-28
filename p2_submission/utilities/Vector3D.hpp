#pragma once
#include <string>
class Point3D;
class Vector3D {
public:
  double x, y, z;
  Vector3D(); Vector3D(double c); Vector3D(double _x, double _y, double _z);
  Vector3D(const Point3D &p);
  Vector3D(const Vector3D &v) = default;
  Vector3D &operator=(const Vector3D &rhs) = default;
  Vector3D &operator=(const Point3D &rhs);
  ~Vector3D() = default;
  std::string to_string() const;
  Vector3D operator-() const;
  Vector3D operator+(const Vector3D &v) const;
  Vector3D &operator+=(const Vector3D &v);
  Vector3D operator-(const Vector3D &v) const;
  Vector3D &operator-=(const Vector3D &v);
  Vector3D operator*(const double a) const;
  Vector3D operator/(const double a) const;
  void normalize();
  double length() const;
  double len_squared() const;
  double operator*(const Vector3D &b) const;
  Vector3D operator^(const Vector3D &v) const;
};
Vector3D operator*(const double a, const Vector3D &v);

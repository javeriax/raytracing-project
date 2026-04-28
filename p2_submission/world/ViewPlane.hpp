#pragma once
#include "../utilities/Point3D.hpp"
#include "../utilities/Vector3D.hpp"
class ViewPlane {
public:
  Point3D top_left, bottom_right;
  Vector3D normal;
  int hres, vres;
  ViewPlane();
  ViewPlane(const ViewPlane &other) = default;
  ViewPlane &operator=(const ViewPlane &rhs) = default;
  int get_hres() const; void set_hres(int);
  int get_vres() const; void set_vres(int);
  ~ViewPlane() = default;
};

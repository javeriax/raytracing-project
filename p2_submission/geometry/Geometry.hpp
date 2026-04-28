#pragma once
#include <string>
class BBox; class Material; class Ray; class ShadeInfo;
class Geometry {
protected:
  Material *material_ptr;
public:
  Geometry();
  Geometry(const Geometry &object) = default;
  Geometry &operator=(const Geometry &rhs) = default;
  virtual ~Geometry() = default;
  virtual std::string to_string() const = 0;
  Material *get_material() const;
  void set_material(Material *mPtr);
  virtual bool hit(const Ray &ray, float &t, ShadeInfo &sinfo) const = 0;
  virtual BBox getBBox() const = 0;
};

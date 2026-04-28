#pragma once
class RGBColor; class ShadeInfo;
class Material {
public:
  Material() = default;
  Material(const Material &other) = default;
  Material &operator=(const Material &other) = default;
  virtual ~Material() = default;
  virtual RGBColor shade(const ShadeInfo &sinfo) const = 0;
};

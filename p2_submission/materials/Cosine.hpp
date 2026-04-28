#pragma once
#include "Material.hpp"
#include "../utilities/RGBColor.hpp"
class Cosine : public Material {
protected:
  RGBColor color;
public:
  Cosine(); Cosine(float c); Cosine(float r, float g, float b); Cosine(const RGBColor &c);
  Cosine(const Cosine &other); Cosine &operator=(const Cosine &other);
  virtual ~Cosine() = default;
  virtual RGBColor shade(const ShadeInfo &sinfo) const override;
};

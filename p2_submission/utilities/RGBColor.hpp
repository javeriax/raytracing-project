#pragma once
#include <string>
class RGBColor {
public:
  float r, g, b;
  RGBColor(); RGBColor(float c); RGBColor(float _r, float _g, float _b);
  ~RGBColor() = default;
  RGBColor(const RGBColor &c) = default;
  RGBColor &operator=(const RGBColor &rhs) = default;
  std::string to_string() const;
  RGBColor operator+(const RGBColor &c) const;
  RGBColor &operator+=(const RGBColor &c);
  RGBColor operator*(const float a) const;
  RGBColor &operator*=(const float a);
  RGBColor operator/(const float a) const;
  RGBColor &operator/=(const float a);
  RGBColor operator*(const RGBColor &c) const;
  bool operator==(const RGBColor &c) const;
  RGBColor powc(float p) const;
  float average() const;
};
RGBColor operator*(const float a, const RGBColor &c);

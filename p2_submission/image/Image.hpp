#pragma once
#include <string>
class RGBColor; class ViewPlane;
class Image {
private:
  int hres, vres;
  RGBColor **colors;
public:
  Image(int hres, int vres);
  Image(const ViewPlane &vp);
  ~Image();
  void set_pixel(int x, int y, const RGBColor &color);
  void write_png(std::string path) const;
};

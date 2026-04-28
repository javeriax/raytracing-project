#include "Cosine.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Vector3D.hpp"
#include <algorithm>

Cosine::Cosine() : color(0, 0, 0) {}
Cosine::Cosine(float c) : color(c, c, c) {}
Cosine::Cosine(float r, float g, float b) : color(r, g, b) {}
Cosine::Cosine(const RGBColor &c) : color(c) {}
Cosine::Cosine(const Cosine &other) : Material(other), color(other.color) {}

Cosine &Cosine::operator=(const Cosine &other) {
    if (this != &other) { Material::operator=(other); color = other.color; }
    return *this;
}

RGBColor Cosine::shade(const ShadeInfo &sinfo) const {
    // cos(theta) = dot(-ray.dir, normal), both unit vectors
    Vector3D neg_dir = -sinfo.ray.d;
    neg_dir.normalize();
    double cos_theta = neg_dir * sinfo.normal;
    // clamp to [0,1]
    if (cos_theta < 0.0) cos_theta = 0.0;
    return color * (float)cos_theta;
}

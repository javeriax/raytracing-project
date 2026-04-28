#include "RGBColor.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>

static float clamp(float v) {
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

RGBColor::RGBColor() : r(0), g(0), b(0) {}
RGBColor::RGBColor(float c) : r(clamp(c)), g(clamp(c)), b(clamp(c)) {}
RGBColor::RGBColor(float _r, float _g, float _b) : r(clamp(_r)), g(clamp(_g)), b(clamp(_b)) {}

std::string RGBColor::to_string() const {
    std::ostringstream o;
    o << "(" << r << ", " << g << ", " << b << ")";
    return o.str();
}

RGBColor RGBColor::operator+(const RGBColor &c) const {
    return RGBColor(r + c.r, g + c.g, b + c.b);
}

RGBColor &RGBColor::operator+=(const RGBColor &c) {
    r = clamp(r + c.r); g = clamp(g + c.g); b = clamp(b + c.b);
    return *this;
}

RGBColor RGBColor::operator*(const float a) const {
    return RGBColor(r * a, g * a, b * a);
}

RGBColor &RGBColor::operator*=(const float a) {
    r = clamp(r * a); g = clamp(g * a); b = clamp(b * a);
    return *this;
}

RGBColor RGBColor::operator/(const float a) const {
    return RGBColor(r / a, g / a, b / a);
}

RGBColor &RGBColor::operator/=(const float a) {
    r = clamp(r / a); g = clamp(g / a); b = clamp(b / a);
    return *this;
}

RGBColor RGBColor::operator*(const RGBColor &c) const {
    return RGBColor(r * c.r, g * c.g, b * c.b);
}

bool RGBColor::operator==(const RGBColor &c) const {
    return r == c.r && g == c.g && b == c.b;
}

RGBColor RGBColor::powc(float p) const {
    return RGBColor(std::pow(r, p), std::pow(g, p), std::pow(b, p));
}

float RGBColor::average() const {
    return (r + g + b) / 3.0f;
}

RGBColor operator*(const float a, const RGBColor &c) {
    return RGBColor(a * c.r, a * c.g, a * c.b);
}

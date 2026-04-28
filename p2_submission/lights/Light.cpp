#include "Light.hpp"

Light::Light() : color(1, 1, 1), intensity(1.0f) {}

Light::Light(const RGBColor& c, float i) : color(c), intensity(i) {}

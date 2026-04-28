#include "PointLight.hpp"
#include "../utilities/ShadeInfo.hpp"

PointLight::PointLight()
    : Light(RGBColor(1, 1, 1), 1.0f), position(0, 0, 0) {}

PointLight::PointLight(const Point3D& pos, const RGBColor& c, float i)
    : Light(c, i), position(pos) {}

// direction is a unit vector pointing FROM hit point TOWARD the light
Vector3D PointLight::get_direction(const ShadeInfo& sinfo) const {
    Vector3D dir = position - sinfo.hit_point;
    dir.normalize();
    return dir;
}

// light color multiplied by intensity
RGBColor PointLight::L(const ShadeInfo& sinfo) const {
    return color * intensity;
}

// euclidean distance from hit point to light position
float PointLight::distance(const ShadeInfo& sinfo) const {
    return (float)(position - sinfo.hit_point).length();
}

#include "DirectionalLight.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Constants.hpp"

DirectionalLight::DirectionalLight()
    : Light(RGBColor(1, 1, 1), 1.0f), direction(0, -1, 0) {}

DirectionalLight::DirectionalLight(const Vector3D &dir, const RGBColor &c, float i)
    : Light(c, i), direction(dir)
{
    // store the negated direction so it points toward the light
    // this matches get_direction convention (toward light)
    this->direction = -dir;
    this->direction.normalize();
}

// same direction everywhere - the sun is so far it's parallel
Vector3D DirectionalLight::get_direction(const ShadeInfo &sinfo) const
{
    return direction;
}

RGBColor DirectionalLight::L(const ShadeInfo &sinfo) const
{
    return color * intensity;
}

// infinitely far - use huge value so shadow ray travels forever
float DirectionalLight::distance(const ShadeInfo &sinfo) const
{
    return kHugeValue;
}

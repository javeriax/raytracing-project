#pragma once

#include "../utilities/RGBColor.hpp"
#include "../utilities/Vector3D.hpp"
#include "../utilities/Point3D.hpp"

class ShadeInfo;
class World;

// abstract base class - all lights inherit from this
class Light {
protected:
    RGBColor color;      // color of the light
    float intensity;     // brightness multiplier

public:
    Light();
    Light(const RGBColor& c, float i);
    virtual ~Light() = default;

    // returns direction from hit point toward the light
    virtual Vector3D get_direction(const ShadeInfo& sinfo) const = 0;

    // returns the light color scaled by intensity
    virtual RGBColor L(const ShadeInfo& sinfo) const = 0;

    // returns distance to light from hit point (huge for directional)
    virtual float distance(const ShadeInfo& sinfo) const = 0;

    // whether this light casts shadows
    virtual bool casts_shadows() const { return true; }
};

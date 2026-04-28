#pragma once

#include "Light.hpp"
#include "../utilities/Point3D.hpp"

// a point light radiates in all directions from a single position
// like a bare lightbulb
class PointLight : public Light {
private:
    Point3D position;    // where the light is in the world

public:
    PointLight();
    PointLight(const Point3D& pos, const RGBColor& c, float intensity);

    // direction from hit point to light position
    virtual Vector3D get_direction(const ShadeInfo& sinfo) const override;

    // light color scaled by intensity
    virtual RGBColor L(const ShadeInfo& sinfo) const override;

    // actual distance from hit point to light
    virtual float distance(const ShadeInfo& sinfo) const override;
};

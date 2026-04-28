#pragma once

#include "Light.hpp"
#include "../utilities/Vector3D.hpp"

// a directional light has no position - it lights everything
// from the same direction equally, like the sun
class DirectionalLight : public Light {
private:
    Vector3D direction;   // direction the light travels (toward scene)

public:
    DirectionalLight();
    DirectionalLight(const Vector3D& dir, const RGBColor& c, float intensity);

    // always returns same direction regardless of hit point
    virtual Vector3D get_direction(const ShadeInfo& sinfo) const override;

    // light color scaled by intensity
    virtual RGBColor L(const ShadeInfo& sinfo) const override;

    // directional lights are infinitely far away
    virtual float distance(const ShadeInfo& sinfo) const override;
};

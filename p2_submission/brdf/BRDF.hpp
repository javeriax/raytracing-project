#pragma once

#include "../utilities/RGBColor.hpp"
#include "../utilities/Vector3D.hpp"

class ShadeInfo;

// brdf = bidirectional reflectance distribution function
// describes how a surface reflects light
// given incoming light direction and outgoing view direction,
// it returns how much light bounces toward the viewer
class BRDF {
public:
    BRDF() = default;
    virtual ~BRDF() = default;

    // evaluates how much light reflects from wi (incoming) to wo (outgoing)
    // wo = direction toward viewer, wi = direction toward light
    virtual RGBColor f(const ShadeInfo& sinfo,
                       const Vector3D& wo,
                       const Vector3D& wi) const = 0;

    // returns the reflectance - overall contribution of this brdf
    virtual RGBColor rho(const ShadeInfo& sinfo,
                         const Vector3D& wo) const = 0;
};

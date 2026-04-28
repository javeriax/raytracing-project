#include "Specular.hpp"
#include "../utilities/ShadeInfo.hpp"

Specular::Specular() : kr(1.0f), cr(1, 1, 1) {}

Specular::Specular(float k, const RGBColor& c) : kr(k), cr(c) {}

// perfect specular - f() returns 0 because reflection is a delta function
// (only reflects in exactly one direction, not a range)
// the actual shading is done via sample_f
RGBColor Specular::f(const ShadeInfo& sinfo,
                     const Vector3D& wo,
                     const Vector3D& wi) const {
    return RGBColor(0);
}

RGBColor Specular::rho(const ShadeInfo& sinfo,
                       const Vector3D& wo) const {
    return cr * kr;
}

// computes the reflected ray direction wr and returns brdf value
// wr = 2(normal . wo) * normal - wo
RGBColor Specular::sample_f(const ShadeInfo& sinfo,
                             const Vector3D& wo,
                             Vector3D& wr) const {
    double ndotwo = sinfo.normal * wo;
    // reflect wo around the normal
    wr = -wo + 2.0 * ndotwo * sinfo.normal;
    wr.normalize();
    return cr * kr;
}

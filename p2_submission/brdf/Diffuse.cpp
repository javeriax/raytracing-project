#include "Diffuse.hpp"
#include "../utilities/Constants.hpp"
#include "../utilities/ShadeInfo.hpp"

Diffuse::Diffuse() : kd(1.0f), cd(1, 1, 1) {}

Diffuse::Diffuse(float k, const RGBColor& c) : kd(k), cd(c) {}

// lambertian brdf is constant - same regardless of view direction
// kd * cd / pi is the physically correct formula
RGBColor Diffuse::f(const ShadeInfo& sinfo,
                    const Vector3D& wo,
                    const Vector3D& wi) const {
    return cd * (kd * invPI);
}

// rho is the hemispherical reflectance - integral of f over hemisphere
// for lambertian this simplifies to kd * cd
RGBColor Diffuse::rho(const ShadeInfo& sinfo,
                      const Vector3D& wo) const {
    return cd * kd;
}

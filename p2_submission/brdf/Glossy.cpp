#include "Glossy.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Constants.hpp"
#include <cmath>

Glossy::Glossy() : ks(0.5f), cs(1, 1, 1), exp(10.0f) {}

Glossy::Glossy(float k, const RGBColor& c, float e) : ks(k), cs(c), exp(e) {}

RGBColor Glossy::f(const ShadeInfo& sinfo,
                   const Vector3D& wo,
                   const Vector3D& wi) const {

    // r = mirror reflection of wi around the surface normal
    // r = 2(normal . wi) * normal - wi
    double ndotwi = sinfo.normal * wi;
    Vector3D r = -wi + 2.0 * ndotwi * sinfo.normal;

    // how close is viewer direction to perfect reflection?
    double rdotwo = r * wo;

    if (rdotwo > 0.0) {
        // phong formula: highlight when view is near mirror direction
        return cs * (ks * (float)std::pow(rdotwo, exp));
    }
    return RGBColor(0);
}

// glossy brdf has no simple closed form rho
// return zero - diffuse handles ambient, glossy only affects direct light
RGBColor Glossy::rho(const ShadeInfo& sinfo,
                     const Vector3D& wo) const {
    return RGBColor(0);
}

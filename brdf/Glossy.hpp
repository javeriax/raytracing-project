#pragma once

#include "BRDF.hpp"
#include "../utilities/RGBColor.hpp"

// glossy specular brdf (phong model)

// shiny surfaces like polished plastic, painted metal etc etc
// reflects light in a lobe around the perfect mirror direction
// exp controls sharpness - higher = tighter/sharper highlight

class Glossy : public BRDF
{
private:
    float ks;    // specular coefficient (0 to 1)
    RGBColor cs; // specular color (usually white/grey for metals)
    float exp;   // phong exponent - higher = shinier

public:
    Glossy();
    Glossy(float k, const RGBColor &c, float e);

    void set_ks(float k) { ks = k; }
    void set_cs(const RGBColor &c) { cs = c; }
    void set_exp(float e) { exp = e; }

    // phong formula: ks * cs * (r . wo)^exp
    // r is the perfect mirror reflection of wi around the normal
    virtual RGBColor f(const ShadeInfo &sinfo,
                       const Vector3D &wo,
                       const Vector3D &wi) const override;

    virtual RGBColor rho(const ShadeInfo &sinfo,
                         const Vector3D &wo) const override;
};

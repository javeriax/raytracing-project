#pragma once

#include "BRDF.hpp"
#include "../utilities/RGBColor.hpp"

// perfect mirror specular brdf
// used for reflective surfaces like mirrors, chrome, still water
// reflects in exactly one direction - the perfect mirror direction
class Specular : public BRDF {
private:
    float kr;         // reflection coefficient (0 to 1)
    RGBColor cr;      // reflection color (white = full color reflection)

public:
    Specular();
    Specular(float k, const RGBColor& c);

    void set_kr(float k) { kr = k; }
    void set_cr(const RGBColor& c) { cr = c; }
    float get_kr() const { return kr; }
    RGBColor get_cr() const { return cr; }

    // perfect specular doesn't use f() the normal way
    // reflection is handled in the material via a reflected ray
    virtual RGBColor f(const ShadeInfo& sinfo,
                       const Vector3D& wo,
                       const Vector3D& wi) const override;

    virtual RGBColor rho(const ShadeInfo& sinfo,
                         const Vector3D& wo) const override;

    // computes the reflected direction and its brdf value
    RGBColor sample_f(const ShadeInfo& sinfo,
                      const Vector3D& wo,
                      Vector3D& wr) const;
};

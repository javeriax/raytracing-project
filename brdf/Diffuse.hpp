#pragma once

#include "BRDF.hpp"
#include "../utilities/RGBColor.hpp"

// lambertian diffuse brdf
// matte surfaces like chalk, clay, unfinished wood
// reflects light equally in all directions regardless of view angle
class Diffuse : public BRDF
{
private:
    float kd;    // diffuse coefficient (0 to 1), controls brightness
    RGBColor cd; // diffuse color of the surface

public:
    Diffuse();
    Diffuse(float k, const RGBColor &c);

    void set_kd(float k) { kd = k; }
    void set_cd(const RGBColor &c) { cd = c; }

    // returns kd * cd / pi
    // dividing by pi makes energy conservation correct
    virtual RGBColor f(const ShadeInfo &sinfo,
                       const Vector3D &wo,
                       const Vector3D &wi) const override;

    // total reflectance = kd * cd
    virtual RGBColor rho(const ShadeInfo &sinfo,
                         const Vector3D &wo) const override;
};

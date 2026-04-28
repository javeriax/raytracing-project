#pragma once

#include "Material.hpp"
#include "../brdf/Diffuse.hpp"
#include "../brdf/Glossy.hpp"
#include "../brdf/Specular.hpp"
#include "../utilities/RGBColor.hpp"

// reflective material - phong + mirror reflections
// like chrome, mirrors, polished metal
// shoots an additional reflected ray to get the mirror color
class Reflective : public Material
{
private:
    Diffuse ambient_brdf;
    Diffuse diffuse_brdf;
    Glossy glossy_brdf;
    Specular specular_brdf; // handles the mirror reflection

public:
    Reflective();
    Reflective(float ka, float kd, float ks,
               float kr, const RGBColor &c, float exp);

    void set_ka(float k);
    void set_kd(float k);
    void set_ks(float k);
    void set_kr(float k);
    void set_cd(const RGBColor &c);
    void set_cr(const RGBColor &c);
    void set_exp(float e);

    virtual RGBColor shade(const ShadeInfo &sinfo) const override;
};

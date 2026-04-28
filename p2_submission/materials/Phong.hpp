#pragma once

#include "Material.hpp"
#include "../brdf/Diffuse.hpp"
#include "../brdf/Glossy.hpp"
#include "../utilities/RGBColor.hpp"

// phong material - diffuse + glossy highlights
// like shiny plastic, painted surfaces, skin
class Phong : public Material {
private:
    Diffuse ambient_brdf;
    Diffuse diffuse_brdf;
    Glossy  specular_brdf;

public:
    Phong();
    Phong(float ka, float kd, float ks, const RGBColor& c, float exp);

    void set_ka(float k);
    void set_kd(float k);
    void set_ks(float k);
    void set_cd(const RGBColor& c);
    void set_cs(const RGBColor& c);
    void set_exp(float e);

    virtual RGBColor shade(const ShadeInfo& sinfo) const override;
};

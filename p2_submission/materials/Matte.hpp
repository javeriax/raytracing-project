#pragma once

#include "Material.hpp"
#include "../brdf/Diffuse.hpp"
#include "../utilities/RGBColor.hpp"

// matte material - purely diffuse, no shininess
// like chalk, clay, matte paint
// uses ambient + diffuse lighting
class Matte : public Material {
private:
    Diffuse diffuse_brdf;   // handles direct diffuse lighting
    Diffuse ambient_brdf;   // handles ambient (indirect) lighting

public:
    Matte();
    Matte(float ka, float kd, const RGBColor& c);

    void set_ka(float k);   // ambient coefficient
    void set_kd(float k);   // diffuse coefficient
    void set_cd(const RGBColor& c);  // surface color

    virtual RGBColor shade(const ShadeInfo& sinfo) const override;
};

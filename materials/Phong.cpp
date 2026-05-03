#include "Phong.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Ray.hpp"
#include "../world/World.hpp"
#include "../lights/Light.hpp"

Phong::Phong()
{
    ambient_brdf.set_kd(0.2f);
    ambient_brdf.set_cd(RGBColor(1, 1, 1));
    diffuse_brdf.set_kd(0.6f);
    diffuse_brdf.set_cd(RGBColor(1, 1, 1));
    specular_brdf.set_ks(0.4f);
    specular_brdf.set_cs(RGBColor(1, 1, 1));
    specular_brdf.set_exp(20.0f);
}

Phong::Phong(float ka, float kd, float ks,
             const RGBColor &c, float exp)
{
    ambient_brdf.set_kd(ka);
    ambient_brdf.set_cd(c);
    diffuse_brdf.set_kd(kd);
    diffuse_brdf.set_cd(c);
    specular_brdf.set_ks(ks);
    specular_brdf.set_cs(RGBColor(1, 1, 1)); // highlights are usually white
    specular_brdf.set_exp(exp);
}

void Phong::set_ka(float k) { ambient_brdf.set_kd(k); }
void Phong::set_kd(float k) { diffuse_brdf.set_kd(k); }
void Phong::set_ks(float k) { specular_brdf.set_ks(k); }
void Phong::set_cd(const RGBColor &c)
{
    ambient_brdf.set_cd(c);
    diffuse_brdf.set_cd(c);
}
void Phong::set_cs(const RGBColor &c) { specular_brdf.set_cs(c); }
void Phong::set_exp(float e) { specular_brdf.set_exp(e); }

// RGBColor Phong::shade(const ShadeInfo &sinfo) const
// {
//     Vector3D wo = -sinfo.ray.d;
//     wo.normalize();

//     // ambient base
//     RGBColor L = ambient_brdf.rho(sinfo, wo) * sinfo.w->ambient_color * sinfo.w->ambient_intensity;

//     for (auto light : sinfo.w->lights)
//     {
//         Vector3D wi = light->get_direction(sinfo);
//         float ndotwi = (float)(sinfo.normal * wi);

//         if (ndotwi > 0.0f)
//         {
//             bool shadowed = false;
//             if (light->casts_shadows())
//             {
//                 Ray shadow_ray(sinfo.hit_point, wi);
//                 shadowed = sinfo.w->in_shadow(shadow_ray, light->distance(sinfo));
//             }

//             if (!shadowed)
//             {
//                 // diffuse + specular combined
//                 L += (diffuse_brdf.f(sinfo, wo, wi) + specular_brdf.f(sinfo, wo, wi)) * light->L(sinfo) * ndotwi;
//             }
//         }
//     }
//     return L;
// }

RGBColor Phong::shade(const ShadeInfo &sinfo) const
{
    // --- NEON ROOF OVERRIDE (Only for the roof strip) ---
    if (is_roof_neon) {
        float y = sinfo.hit_point.y;
        
        // Hardcoded neon colors to avoid "get_cd" errors
        RGBColor neon_p(1.5f, 0.0f, 0.65f);
        RGBColor neon_b(0.0f, 0.3f, 1.5f);

        // Pink Strip (Top half)
        if (y > -3.20f) return neon_p;
        
        // Small Gap (Middle) - creates the "two strip" look
        if (y <= -3.20f && y > -3.25f) return RGBColor(0, 0, 0);
        
        // Blue Strip (Bottom half)
        return neon_b;
    }

    // --- YOUR ORIGINAL PHONG CODE START ---
    Vector3D wo = -sinfo.ray.d;
    wo.normalize();

    RGBColor L = ambient_brdf.rho(sinfo, wo) * sinfo.w->ambient_color * sinfo.w->ambient_intensity;

    for (auto light : sinfo.w->lights)
    {
        Vector3D wi = light->get_direction(sinfo);
        float ndotwi = (float)(sinfo.normal * wi);

        if (ndotwi > 0.0f)
        {
            bool shadowed = false;
            if (light->casts_shadows())
            {
                Ray shadow_ray(sinfo.hit_point, wi);
                shadowed = sinfo.w->in_shadow(shadow_ray, light->distance(sinfo));
            }

            if (!shadowed)
            {
                L += (diffuse_brdf.f(sinfo, wo, wi) + specular_brdf.f(sinfo, wo, wi)) * light->L(sinfo) * ndotwi;
            }
        }
    }
    return L;
}
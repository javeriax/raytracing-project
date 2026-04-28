#include "Reflective.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Ray.hpp"
#include "../world/World.hpp"
#include "../lights/Light.hpp"
#include "../tracers/Tracer.hpp"

Reflective::Reflective()
{
    ambient_brdf.set_kd(0.1f);
    ambient_brdf.set_cd(RGBColor(1, 1, 1));
    diffuse_brdf.set_kd(0.4f);
    diffuse_brdf.set_cd(RGBColor(1, 1, 1));
    glossy_brdf.set_ks(0.3f);
    glossy_brdf.set_cs(RGBColor(1, 1, 1));
    glossy_brdf.set_exp(50.0f);
    specular_brdf.set_kr(0.5f);
    specular_brdf.set_cr(RGBColor(1, 1, 1));
}

Reflective::Reflective(float ka, float kd, float ks,
                       float kr, const RGBColor &c, float exp)
{
    ambient_brdf.set_kd(ka);
    ambient_brdf.set_cd(c);
    diffuse_brdf.set_kd(kd);
    diffuse_brdf.set_cd(c);
    glossy_brdf.set_ks(ks);
    glossy_brdf.set_cs(RGBColor(1, 1, 1));
    glossy_brdf.set_exp(exp);
    specular_brdf.set_kr(kr);
    specular_brdf.set_cr(RGBColor(1, 1, 1));
}

void Reflective::set_ka(float k) { ambient_brdf.set_kd(k); }
void Reflective::set_kd(float k) { diffuse_brdf.set_kd(k); }
void Reflective::set_ks(float k) { glossy_brdf.set_ks(k); }
void Reflective::set_kr(float k) { specular_brdf.set_kr(k); }
void Reflective::set_cd(const RGBColor &c)
{
    ambient_brdf.set_cd(c);
    diffuse_brdf.set_cd(c);
}
void Reflective::set_cr(const RGBColor &c) { specular_brdf.set_cr(c); }
void Reflective::set_exp(float e) { glossy_brdf.set_exp(e); }

RGBColor Reflective::shade(const ShadeInfo &sinfo) const
{
    Vector3D wo = -sinfo.ray.d;
    wo.normalize();

    // ambient base lighting
    RGBColor L = ambient_brdf.rho(sinfo, wo) * sinfo.w->ambient_color * sinfo.w->ambient_intensity;

    // direct lighting from each light source
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
                L += (diffuse_brdf.f(sinfo, wo, wi) + glossy_brdf.f(sinfo, wo, wi)) * light->L(sinfo) * ndotwi;
            }
        }
    }

    // mirror reflection - shoot a new ray in reflected direction
    if (sinfo.depth < 5 && sinfo.w->tracer_ptr)
    {
        Vector3D wr;
        RGBColor fr = specular_brdf.sample_f(sinfo, wo, wr);

        // create reflected ray starting from hit point
        Ray reflected_ray(sinfo.hit_point, wr);

        // create new shadeinfo with increased depth to prevent infinite reflections
        ShadeInfo reflected_sinfo(*sinfo.w);
        reflected_sinfo.depth = sinfo.depth + 1;
        reflected_sinfo.ray = reflected_ray;

        // trace the reflected ray and add its color
        float ndotwr = (float)(sinfo.normal * wr);
        L += fr * sinfo.w->tracer_ptr->trace_ray(reflected_ray, sinfo.depth + 1) * ndotwr;
    }

    return L;
}

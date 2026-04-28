#include "Matte.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Ray.hpp"
#include "../world/World.hpp"
#include "../lights/Light.hpp"

Matte::Matte() {
    ambient_brdf.set_kd(0.2f);
    ambient_brdf.set_cd(RGBColor(1, 1, 1));
    diffuse_brdf.set_kd(0.8f);
    diffuse_brdf.set_cd(RGBColor(1, 1, 1));
}

Matte::Matte(float ka, float kd, const RGBColor& c) {
    ambient_brdf.set_kd(ka);
    ambient_brdf.set_cd(c);
    diffuse_brdf.set_kd(kd);
    diffuse_brdf.set_cd(c);
}

void Matte::set_ka(float k) { ambient_brdf.set_kd(k); }
void Matte::set_kd(float k) { diffuse_brdf.set_kd(k); }
void Matte::set_cd(const RGBColor& c) {
    ambient_brdf.set_cd(c);
    diffuse_brdf.set_cd(c);
}

RGBColor Matte::shade(const ShadeInfo& sinfo) const {
    // wo = direction from hit point toward the camera
    Vector3D wo = -sinfo.ray.d;
    wo.normalize();

    // ambient contribution - base light that hits everything
    RGBColor L = ambient_brdf.rho(sinfo, wo)
               * sinfo.w->ambient_color
               * sinfo.w->ambient_intensity;

    // add contribution from each light source
    for (auto light : sinfo.w->lights) {
        Vector3D wi = light->get_direction(sinfo);
        float ndotwi = (float)(sinfo.normal * wi);

        if (ndotwi > 0.0f) {
            // check if this point is in shadow from this light
            bool shadowed = false;
            if (light->casts_shadows()) {
                Ray shadow_ray(sinfo.hit_point, wi);
                float dist = light->distance(sinfo);
                shadowed = sinfo.w->in_shadow(shadow_ray, dist);
            }

            if (!shadowed) {
                // diffuse: brdf * light_color * cos(angle)
                L += diffuse_brdf.f(sinfo, wo, wi)
                   * light->L(sinfo)
                   * ndotwi;
            }
        }
    }
    return L;
}

#include "ShadowCaster.hpp"
#include "../world/World.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../materials/Material.hpp"

ShadowCaster::ShadowCaster(World* w) : Tracer(w) {}

RGBColor ShadowCaster::trace_ray(const Ray& ray, int depth) const {
    ShadeInfo sinfo = world_ptr->hit_objects(ray);

    if (sinfo.hit) {
        // pass depth so reflective materials know recursion level
        sinfo.ray = ray;
        sinfo.depth = depth;
        // shade() in each material handles shadow ray logic itself
        return sinfo.material_ptr->shade(sinfo);
    }
    return world_ptr->bg_color;
}

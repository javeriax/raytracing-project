#include "Basic.hpp"
#include "../world/World.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../materials/Material.hpp"

Basic::Basic(World* w) : Tracer(w) {}

RGBColor Basic::trace_ray(const Ray& ray, int depth) const {
    ShadeInfo sinfo = world_ptr->hit_objects(ray);

    if (sinfo.hit) {
        // let the material decide the color
        sinfo.ray = ray;
        sinfo.depth = depth;
        return sinfo.material_ptr->shade(sinfo);
    }
    // ray hit nothing - return background color
    return world_ptr->bg_color;
}

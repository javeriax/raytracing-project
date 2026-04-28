#include "Jittered.hpp"
#include "../cameras/Camera.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/Point3D.hpp"
#include "../utilities/Vector3D.hpp"
#include "../utilities/Constants.hpp"
#include "../world/ViewPlane.hpp"
#include <cstdlib>

Jittered::Jittered(Camera *c_ptr, ViewPlane *v_ptr, int grid_size)
    : Sampler(c_ptr, v_ptr), n(grid_size) {}

Jittered::Jittered(const Jittered &other)
    : Sampler(other), n(other.n) {}

Jittered &Jittered::operator=(const Jittered &other)
{
    if (this != &other)
    {
        Sampler::operator=(other);
        n = other.n;
    }
    return *this;
}

std::vector<Ray> Jittered::get_rays(int px, int py) const
{
    std::vector<Ray> rays;
    rays.reserve(n * n);

    float hres = (float)viewplane_ptr->hres;
    float vres = (float)viewplane_ptr->vres;

    // size of each subcell within a pixel
    float cell_size = 1.0f / n;

    // weight of each ray = 1 / total rays
    float weight = 1.0f / (float)(n * n);

    Point3D tl = viewplane_ptr->top_left;
    Point3D br = viewplane_ptr->bottom_right;

    // shoot n*n rays, one per subcell
    for (int i = 0; i < n; i++)
    { // subcell column
        for (int j = 0; j < n; j++)
        { // subcell row

            // random offset within this subcell (0 to cell_size)
            float rand_u = (float)rand() / (float)RAND_MAX;
            float rand_v = (float)rand() / (float)RAND_MAX;

            // position within pixel: subcell offset + random jitter
            float u = (px + (i + rand_u) * cell_size) / hres;
            float v = (py + (j + rand_v) * cell_size) / vres;

            // map to world space on view plane
            float wx = tl.x + u * (br.x - tl.x);
            float wy = tl.y + v * (br.y - tl.y);
            float wz = tl.z + v * (br.z - tl.z);

            Point3D pixel_pos(wx, wy, wz);

            Vector3D dir = camera_ptr->get_direction(pixel_pos);
            dir.normalize();

            Ray ray(pixel_pos, dir);
            ray.w = weight; // each ray contributes equally
            rays.push_back(ray);
        }
    }
    return rays;
}

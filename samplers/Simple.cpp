#include "Simple.hpp"
#include "../cameras/Camera.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/Point3D.hpp"
#include "../utilities/Vector3D.hpp"
#include "../world/ViewPlane.hpp"

Simple::Simple(Camera *c_ptr, ViewPlane *v_ptr) : Sampler(c_ptr, v_ptr) {}

Simple::Simple(const Simple &other) : Sampler(other) {}

Simple &Simple::operator=(const Simple &other)
{
    if (this != &other)
        Sampler::operator=(other);
    return *this;
}

std::vector<Ray> Simple::get_rays(int px, int py) const
{

    float hres = (float)viewplane_ptr->hres;
    float vres = (float)viewplane_ptr->vres;

    // interpolate across vview plane
    float u = (px + 0.5f) / hres; // [0,1] horizontal
    float v = (py + 0.5f) / vres; // [0,1] vertical

    Point3D tl = viewplane_ptr->top_left;
    Point3D br = viewplane_ptr->bottom_right;

    float wx = tl.x + u * (br.x - tl.x);
    float wy = tl.y + v * (br.y - tl.y);
    float wz = tl.z + v * (br.z - tl.z); // z may vary for perspective

    Point3D pixel_center(wx, wy, wz);

    Vector3D dir = camera_ptr->get_direction(pixel_center);
    dir.normalize();

    Ray ray(pixel_center, dir);
    ray.w = 1.0f;

    return {ray};
}

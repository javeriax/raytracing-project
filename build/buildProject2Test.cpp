// test scene for project 2
// tests: matte material, phong material, reflective material,
//        point light, directional light, shadows, bvh acceleration

#include "../cameras/Perspective.hpp"
#include "../geometry/Sphere.hpp"
#include "../geometry/Plane.hpp"
#include "../materials/Matte.hpp"
#include "../materials/Phong.hpp"
#include "../materials/Reflective.hpp"
#include "../lights/PointLight.hpp"
#include "../lights/DirectionalLight.hpp"
#include "../samplers/Jittered.hpp"
#include "../tracers/ShadowCaster.hpp"
#include "../acceleration/BVH.hpp"
#include "../utilities/Constants.hpp"
#include "../world/World.hpp"

void World::build(void) {
    // view plane setup
    vplane.top_left.x     = -10;
    vplane.top_left.y     =  10;
    vplane.top_left.z     =  10;
    vplane.bottom_right.x =  10;
    vplane.bottom_right.y = -10;
    vplane.bottom_right.z =  10;
    vplane.hres = 400;
    vplane.vres = 400;

    bg_color = RGBColor(0.05f, 0.05f, 0.1f);  // dark blue background
    ambient_color     = RGBColor(1, 1, 1);
    ambient_intensity = 0.15f;

    // perspective camera looking at origin from front
    set_camera(new Perspective(0, 2, 20));

    // jittered sampler with 2x2 grid = 4 rays per pixel
    // use Simple(camera_ptr, &vplane) instead to disable antialiasing
    sampler_ptr = new Jittered(camera_ptr, &vplane, 2);

    // shadow caster tracer for full lighting with shadows
    // use Basic(this) instead to disable shadows
    set_tracer(new ShadowCaster(this));

    // bvh acceleration structure
    // set_acceleration(nullptr) to disable and compare timing
    set_acceleration(new BVH(this));

    // lights
    // point light above and to the left
    add_light(new PointLight(Point3D(-5, 10, 10),
                             RGBColor(1, 1, 1), 2.0f));

    // warm directional light from upper right (like sunlight)
    add_light(new DirectionalLight(Vector3D(1, -1, -1),
                                   RGBColor(1, 0.9f, 0.7f), 0.8f));

    // matte red sphere on the left
    Sphere* s1 = new Sphere(Point3D(-4, 0, 0), 2);
    s1->set_material(new Matte(0.2f, 0.8f, RGBColor(0.8f, 0.1f, 0.1f)));
    add_geometry(s1);

    // shiny blue phong sphere in the center
    Sphere* s2 = new Sphere(Point3D(0, 0, 0), 2);
    Phong* phong = new Phong();
    phong->set_ka(0.2f);
    phong->set_kd(0.5f);
    phong->set_ks(0.6f);
    phong->set_cd(RGBColor(0.1f, 0.2f, 0.9f));
    phong->set_exp(80.0f);
    s2->set_material(phong);
    add_geometry(s2);

    // reflective chrome sphere on the right
    Sphere* s3 = new Sphere(Point3D(4, 0, 0), 2);
    s3->set_material(new Reflective(0.1f, 0.3f, 0.4f,
                                    0.8f, RGBColor(0.9f, 0.9f, 0.9f), 100.0f));
    add_geometry(s3);

    // grey matte floor plane
    Plane* floor = new Plane(Point3D(0, -2, 0), Vector3D(0, 1, 0));
    floor->set_material(new Matte(0.2f, 0.8f, RGBColor(0.5f, 0.5f, 0.5f)));
    add_geometry(floor);
}

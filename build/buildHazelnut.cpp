// hazelnut bush test scene
// tests: obj loading, bvh with real geometry, matte + phong materials
// model is ~79k triangles - good test for bvh performance

#include "../cameras/Perspective.hpp"
#include "../geometry/OBJLoader.hpp"
#include "../geometry/Plane.hpp"
#include "../materials/Matte.hpp"
#include "../materials/Phong.hpp"
#include "../lights/PointLight.hpp"
#include "../lights/DirectionalLight.hpp"
#include "../samplers/Simple.hpp"
#include "../tracers/ShadowCaster.hpp"
#include "../acceleration/BVH.hpp"
#include "../utilities/Constants.hpp"
#include "../world/World.hpp"

void World::build(void)
{
    // model is ~14 wide, ~19 tall, centered around x=0, z=0
    // camera positioned to see the whole bush from the front

    vplane.top_left.x = -12;
    vplane.top_left.y = 16;
    vplane.top_left.z = 10;
    vplane.bottom_right.x = 12;
    vplane.bottom_right.y = -4;
    vplane.bottom_right.z = 10;
    vplane.hres = 400;
    vplane.vres = 400;

    // soft sky blue background
    bg_color = RGBColor(0.53f, 0.81f, 0.92f);
    ambient_color = RGBColor(1.0f, 1.0f, 0.9f);
    ambient_intensity = 0.3f;

    // camera looks at center of bush (height ~9.6 = half of 19.2)
    // positioned far enough back to see the whole thing
    set_camera(new Perspective(0, 10, 35));

    // simple sampler for fast test render - swap to jittered for quality
    sampler_ptr = new Simple(camera_ptr, &vplane);

    // shadow caster for proper lighting
    set_tracer(new ShadowCaster(this));

    // bvh is essential here - 79k triangles without it = very slow
    set_acceleration(new BVH(this));

    // sunlight from upper right - warm daylight color
    add_light(new DirectionalLight(
        Vector3D(-1, -1.5f, -1),
        RGBColor(1.0f, 0.95f, 0.8f), 1.2f));

    // soft fill light from the left to reduce harsh shadows
    add_light(new PointLight(
        Point3D(-20, 15, 20),
        RGBColor(0.6f, 0.7f, 1.0f), 1.5f));

    // ground plane - earthy brown
    Plane *ground = new Plane(Point3D(0, 0, 0), Vector3D(0, 1, 0));
    ground->set_material(
        new Matte(0.2f, 0.8f, RGBColor(0.35f, 0.25f, 0.15f)));
    add_geometry(ground);

    // hazelnut bush material - green leaves with slight sheen
    Phong *leaf_mat = new Phong();
    leaf_mat->set_ka(0.3f);
    leaf_mat->set_kd(0.7f);
    leaf_mat->set_ks(0.2f);
    leaf_mat->set_cd(RGBColor(0.15f, 0.5f, 0.1f)); // leaf green
    leaf_mat->set_exp(15.0f);                      // soft highlight, not too shiny

    // load the obj - path relative to where you run the exe from
    load_obj("Hazelnut.obj", *this, leaf_mat);
}           
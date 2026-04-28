#include "../cameras/Perspective.hpp"
#include "../geometry/OBJLoader.hpp"
#include "../geometry/Plane.hpp"
#include "../materials/Matte.hpp"
#include "../materials/Phong.hpp"
#include "../materials/Reflective.hpp"
#include "../lights/PointLight.hpp"
#include "../lights/DirectionalLight.hpp"
#include "../samplers/Simple.hpp"
#include "../tracers/ShadowCaster.hpp"
#include "../acceleration/BVH.hpp"
#include "../utilities/Constants.hpp"
#include "../world/World.hpp"

void World::build(void)
{
    // wide frame - bike body is 8 long, 4 tall
    // bush scaled to 0.5 = 7 wide, 9.6 tall
    vplane.top_left.x = -12;
    vplane.top_left.y = 9;
    vplane.top_left.z = 10;
    vplane.bottom_right.x = 12;
    vplane.bottom_right.y = -3;
    vplane.bottom_right.z = 10;
    vplane.hres = 480;
    vplane.vres = 270;

    bg_color = RGBColor(0.55f, 0.65f, 0.75f);
    ambient_color = RGBColor(0.85f, 0.9f, 1.0f);
    ambient_intensity = 0.3f;

    // camera pulled back enough to see 8-unit bike + 7-unit bush side by side
    set_camera(new Perspective(0, 3, 18));
    sampler_ptr = new Simple(camera_ptr, &vplane);
    set_tracer(new ShadowCaster(this));
    set_acceleration(new BVH(this));

    // warm directional sun from upper right
    add_light(new DirectionalLight(
        Vector3D(-0.5f, -1.0f, -0.3f),
        RGBColor(1.0f, 0.93f, 0.75f), 1.5f));

    // cool sky fill from above
    add_light(new PointLight(
        Point3D(0, 20, 12),
        RGBColor(0.6f, 0.75f, 1.0f), 2.0f));

    // grey asphalt road plane
    Plane *road = new Plane(Point3D(0, 0, 0), Vector3D(0, 1, 0));
    road->set_material(
        new Matte(0.15f, 0.65f, RGBColor(0.28f, 0.28f, 0.30f)));
    add_geometry(road);

    // hazelnut bush - left side, behind motorcycle
    // scale 0.5: 7.35 wide, 9.6 tall - good tree size
    Phong *leaf_mat = new Phong();
    leaf_mat->set_ka(0.25f);
    leaf_mat->set_kd(0.65f);
    leaf_mat->set_ks(0.15f);
    leaf_mat->set_cd(RGBColor(0.12f, 0.42f, 0.08f));
    leaf_mat->set_exp(12.0f);

    // move left (-7) and slightly behind (+2 in z)
    OBJTransform bush_t(0.5f, -7.0f, 0.0f, 2.0f);
    load_obj("Hazelnut.obj", *this, leaf_mat, bush_t);

    // motorcycle - no x scale needed, body is already 8 units
    // scale=1, shift y=-2.17 to place wheels on ground
    // center x: body center at x=1.23, shift by -1.23 to center
    // then move right by 3 to place next to bush
    Reflective *chrome = new Reflective(
        0.1f, 0.25f, 0.5f, 0.7f,
        RGBColor(0.85f, 0.85f, 0.9f), 200.0f);

    // x offset: +3 (right of center), y: -2.17 (on ground), z: -1 (forward)
    OBJTransform moto_t(1.0f, 1.77f, -2.17f, -1.0f);
    load_obj("moto_simple_1.obj", *this, chrome, moto_t);
}
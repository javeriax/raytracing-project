// Light Test Scene
// Tests different light configurations to validate lighting system
// Compare renders with different light setups to see the effects

#include "../cameras/Perspective.hpp"
#include "../geometry/Plane.hpp"
#include "../geometry/Sphere.hpp"
#include "../materials/Matte.hpp"
#include "../materials/Phong.hpp"
#include "../materials/Reflective.hpp"
#include "../lights/PointLight.hpp"
#include "../lights/DirectionalLight.hpp"
#include "../samplers/Simple.hpp"
#include "../samplers/Jittered.hpp"
#include "../tracers/ShadowCaster.hpp"
#include "../acceleration/BVH.hpp"
#include "../world/World.hpp"

void World::build(void)
{
    // ===== VIEW PLANE =====
    vplane.top_left.x = -10;
    vplane.top_left.y = 10;
    vplane.top_left.z = 15;
    vplane.bottom_right.x = 10;
    vplane.bottom_right.y = -5;
    vplane.bottom_right.z = 15;
    vplane.hres = 800;
    vplane.vres = 600;

    // ===== CAMERA =====
    Perspective *camera = new Perspective(0, 5, 20); // Camera at (0, 5, 20)
    set_camera(camera);

    // ===== SAMPLER =====
    // Use Jittered for anti-aliasing (4x4 grid = 16 rays per pixel)
    sampler_ptr = new Jittered(camera_ptr, &vplane, 4);

    // ===== BACKGROUND & AMBIENT =====
    bg_color = RGBColor(0.1f, 0.1f, 0.15f); // Dark blue-grey background
    ambient_color = RGBColor(1, 1, 1);
    ambient_intensity = 0.5f; // Base ambient light level

    // ===== LIGHTS - CHANGE THESE TO TEST DIFFERENT CONFIGURATIONS =====

    // CONFIGURATION 1: Single Point Light (GOOD FOR TESTING)
    lights.push_back(new PointLight(
        Point3D(5, 15, 10), // Move higher and closer
        RGBColor(1, 1, 1),
        1.5f // Increase intensity
        ));

    // CONFIGURATION 2: Add a Directional Light (simulates sun)
    // Uncomment to add sun-like lighting from the side
    // lights.push_back(new DirectionalLight(
    //     Vector3D(-1, -0.5f, -1),        // Direction: coming from upper left
    //     RGBColor(0.8f, 0.8f, 0.7f),    // Color: warm white
    //     0.6f                            // Intensity
    // ));

    // CONFIGURATION 3: Multiple Point Lights (colorful test)
    // Uncomment to test multiple colored lights
    // lights.push_back(new PointLight(Point3D(8, 8, 0), RGBColor(1, 0, 0), 0.7f));     // Red
    // lights.push_back(new PointLight(Point3D(-8, 8, 0), RGBColor(0, 1, 0), 0.7f));    // Green
    // lights.push_back(new PointLight(Point3D(0, 8, 8), RGBColor(0, 0, 1), 0.7f));     // Blue

    // ===== MATERIALS =====
    Matte *matte_grey = new Matte(0.2f, 0.8f, RGBColor(0.5f, 0.5f, 0.5f));
    Phong *phong_red = new Phong(0.1f, 0.6f, 0.3f, RGBColor(1, 0.2f, 0.2f), 50);
    Phong *phong_blue = new Phong(0.1f, 0.6f, 0.3f, RGBColor(0.2f, 0.5f, 1), 50);
    Reflective *reflective_white = new Reflective(0.05f, 0.2f, 0.3f, 0.7f, RGBColor(0.9f, 0.95f, 1), 100);
    Matte *matte_white = new Matte(0.2f, 0.9f, RGBColor(0.95f, 0.95f, 0.95f));

    // ===== GEOMETRY =====

    // Ground plane
    Plane *ground = new Plane(Point3D(0, 0, 0), Vector3D(0, 1, 0));
    ground->set_material(matte_grey);
    add_geometry(ground);

    // Red Phong sphere - shows specular highlights from light
    Sphere *red_sphere = new Sphere(Point3D(-5, 2, 0), 2);
    red_sphere->set_material(phong_red);
    add_geometry(red_sphere);

    // Blue Phong sphere - shows specular highlights
    Sphere *blue_sphere = new Sphere(Point3D(5, 2, 0), 2);
    blue_sphere->set_material(phong_blue);
    add_geometry(blue_sphere);

    // Reflective sphere - shows mirror reflections of light and surroundings
    Sphere *mirror_sphere = new Sphere(Point3D(0, 3, -4), 1.5f);
    mirror_sphere->set_material(reflective_white);
    add_geometry(mirror_sphere);

    // Small white sphere to the side - easier to see shadow boundaries
    Sphere *white_sphere = new Sphere(Point3D(-4, 1, 4), 0.8f);
    white_sphere->set_material(matte_white);
    add_geometry(white_sphere);

    // ===== TRACER & ACCELERATION =====
    // Use ShadowCaster tracer for full lighting with shadows and reflections
    tracer_ptr = new ShadowCaster(this);

    // Use BVH acceleration structure for faster ray-geometry intersections
    acceleration_ptr = new BVH(this);
    build_acceleration();
}

/*
 * TESTING INSTRUCTIONS:
 *
 * 1. BASELINE TEST (current):
 *    - Single PointLight at (8, 10, 8)
 *    - Look for: Shadows cast by spheres onto ground
 *    - Observe: Bright lit sides, dark shadowed sides
 *
 * 2. ADD DIRECTIONAL LIGHT:
 *    - Uncomment CONFIGURATION 2
 *    - Recompile and run
 *    - Look for: Additional fill light from sun direction
 *    - Observe: Smoother gradation, less harsh shadows
 *
 * 3. MULTIPLE COLORED LIGHTS:
 *    - Uncomment CONFIGURATION 3, comment out CONFIGURATION 1
 *    - Recompile and run
 *    - Look for: Color mixing on objects and ground (shadow regions show color)
 *    - Observe: Red shadow where green blocks red light, etc.
 *
 * 4. NO LIGHTS (AMBIENT ONLY):
 *    - Comment out all light additions
 *    - Look for: Uniform ambient lighting only (no highlights, no shadows)
 *
 * 5. COMPARE SHADOW QUALITY:
 *    - Toggle between Simple and Jittered sampler
 *    - Jittered should show smoother shadow edges (anti-aliasing)
 *
 * 6. MEASURE PERFORMANCE:
 *    - Time renders with/without BVH acceleration
 *    - BVH should be much faster for complex scenes
 */


#include "../cameras/Perspective.hpp"
#include "../geometry/OBJLoader.hpp"
#include "../geometry/Plane.hpp"
#include "../materials/Matte.hpp"
#include "../materials/Phong.hpp"
#include "../materials/Reflective.hpp"
#include "../lights/PointLight.hpp"
#include "../lights/DirectionalLight.hpp"
#include "../samplers/Simple.hpp"
#include "../samplers/Jittered.hpp"
#include "../tracers/ShadowCaster.hpp"
#include "../acceleration/BVH.hpp"
#include "../utilities/Constants.hpp"
#include "../world/World.hpp"
#include "../geometry/Sphere.hpp"
#include <map>
#include <string>

void World::build(void)
{

    vplane.top_left.x = -2.24f;
    vplane.top_left.y = -3.22f;
    vplane.top_left.z = 2.0f;
    vplane.bottom_right.x = 4.76f;
    vplane.bottom_right.y = -8.47f;
    vplane.bottom_right.z = 2.0f;

    // alter camerae and pperspective here

    set_camera(new Perspective(3.0f, -5.0f, 8.0f)); // a bit from right, shows inside lights too
    // set_camera(new Perspective(7.0, -8.0, 4.0));     //opposite side
    // set_camera(new Perspective(3.0f, -5.0f, 7.0f));  //left

    // alter resolution and jitter here:

    // // LOW RESOLUTION FOR WITHOUT BVH
    // vplane.hres = 120;
    // vplane.vres = 90;
    // sampler_ptr = new Jittered(camera_ptr, &vplane, 1);

    // LOW RESOLUTION
    vplane.hres = 480;
    vplane.vres = 360;
    sampler_ptr = new Jittered(camera_ptr, &vplane, 2);

    // //HIGH RESOLUTION
    // vplane.hres = 1920;
    // vplane.vres = 1080;
    // sampler_ptr = new Jittered(camera_ptr, &vplane, 6);

    // bg_color          = RGBColor(0.0f, 0.0f, 0.0f);
    bg_color = RGBColor(0.005f, 0.005f, 0.02f);
    ambient_color = RGBColor(1.0f, 0.95f, 0.85f);
    ambient_intensity = 0.45f;
    set_tracer(new ShadowCaster(this));
    set_acceleration(new BVH(this));

    // stars (they are in the wide angle rendere, very few show)

    Matte *mat_star = new Matte();
    mat_star->set_ka(3.0f); // bright
    mat_star->set_cd(RGBColor(1.2f, 1.2f, 1.2f));

    srand(42);
    for (int i = 0; i < 40; i++)
    {
        float x = -100.0f + (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 140.0f)));
        float y = 4.0f + (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 30.0f)));
        float z = -80.0f;

        Sphere *s = new Sphere(Point3D(x, y, z), 0.06f);
        s->set_material(mat_star);
        add_geometry(s);
    }

    RGBColor warm(1.0f, 0.92f, 0.75f);
    float canopy_i = 250.0f;

    // Highlights the dark crevices of the bike engine and car bumper.
    add_light(new PointLight(Point3D(2.0f, -6.5f, 2.0f), RGBColor(0.8f, 0.85f, 1.0f), 120.0f));

    // Canopy lights at Y=-3.60 (below ceiling) for clear downward illumination
    add_light(new PointLight(Point3D(-1.29f, -3.60f, -10.28f), warm, canopy_i));
    add_light(new PointLight(Point3D(1.29f, -3.60f, -10.28f), warm, canopy_i));
    add_light(new PointLight(Point3D(1.29f, -3.60f, -8.54f), warm, canopy_i));
    add_light(new PointLight(Point3D(1.29f, -3.60f, -5.06f), warm, canopy_i));
    add_light(new PointLight(Point3D(-1.29f, -3.60f, -8.54f), warm, canopy_i));
    add_light(new PointLight(Point3D(-1.28f, -3.60f, -5.06f), warm, canopy_i));
    add_light(new PointLight(Point3D(-1.29f, -3.60f, -6.80f), warm, canopy_i));
    add_light(new PointLight(Point3D(1.29f, -3.60f, -6.80f), warm, canopy_i));

    // Broad fill lights — station left wing and right side
    add_light(new PointLight(Point3D(-8.0f, -3.6f, -7.0f), warm, 120.0f));
    add_light(new PointLight(Point3D(-8.0f, -3.6f, -12.0f), warm, 120.0f));
    add_light(new PointLight(Point3D(5.0f, -3.6f, -7.0f), warm, 80.0f));
    add_light(new PointLight(Point3D(5.0f, -3.6f, -12.0f), warm, 80.0f));

    // Extra fill lights near ground to brighten under-canopy floor
    add_light(new PointLight(Point3D(0.0f, -6.5f, -7.5f), warm, 40.0f));
    add_light(new PointLight(Point3D(0.0f, -6.5f, -10.0f), warm, 40.0f));
    add_light(new PointLight(Point3D(-4.0f, -6.5f, -8.0f), warm, 30.0f));

    // Strong directional downward ; canopy ceiling glow
    add_light(new DirectionalLight(
        Vector3D(0.0f, -1.0f, 0.0f),
        RGBColor(1.0f, 0.92f, 0.75f), 3.0f));

    // Blue moonlight from behind and slightly above camera
    add_light(new DirectionalLight(
        Vector3D(0.0f, -0.3f, -1.0f),
        RGBColor(0.12f, 0.14f, 0.28f), 0.6f));

    RGBColor neon_pink(1.5f, 0.0f, 0.65f);

    // Pink:  along front roof edge, moved inward to actually hit canopy geometry
    add_light(new PointLight(Point3D(-4.0f, -3.20f, -2.0f), neon_pink, 400.0f));
    add_light(new PointLight(Point3D(0.0f, -3.20f, -2.0f), neon_pink, 400.0f));

    // MATERIALS — mapped to usemtl names from MTL file

    // Default fallback for unmapped materials
    Phong *mat_default = new Phong();
    mat_default->set_ka(0.15f);
    mat_default->set_kd(0.55f);
    mat_default->set_ks(0.3f);
    mat_default->set_exp(50.0f);
    mat_default->set_cd(RGBColor(0.5f, 0.48f, 0.45f));
    mat_default->set_cs(RGBColor(1.0f, 0.95f, 0.85f));

    // BIKE GREY-SCALE VARIATIONS

    // Light Metallic Grey — Body/Tank
    Reflective *mat_bike_body = new Reflective();
    mat_bike_body->set_ka(0.15f);
    mat_bike_body->set_kd(0.4f);
    mat_bike_body->set_ks(0.8f);
    mat_bike_body->set_kr(0.2f);
    mat_bike_body->set_exp(200.0f);
    mat_bike_body->set_cd(RGBColor(0.55f, 0.55f, 0.58f));
    mat_bike_body->set_cr(RGBColor(0.7f, 0.7f, 0.8f));

    // Medium Steel — Engine
    Phong *mat_bike_engine = new Phong();
    mat_bike_engine->set_ka(0.2f);
    mat_bike_engine->set_kd(0.5f);
    mat_bike_engine->set_ks(0.4f);
    mat_bike_engine->set_exp(50.0f);
    mat_bike_engine->set_cd(RGBColor(0.35f, 0.35f, 0.37f));

    // Dark Gunmetal — Frame
    Phong *mat_bike_frame = new Phong();
    mat_bike_frame->set_ka(0.1f);
    mat_bike_frame->set_kd(0.3f);
    mat_bike_frame->set_ks(0.6f);
    mat_bike_frame->set_exp(150.0f);
    mat_bike_frame->set_cd(RGBColor(0.15f, 0.15f, 0.18f));

    // Matte Charcoal — Non-reflective parts
    Matte *mat_charcoal = new Matte();
    mat_charcoal->set_ka(0.15f);
    mat_charcoal->set_kd(0.4f);
    mat_charcoal->set_cd(RGBColor(0.1f, 0.1f, 0.1f));

    // 1. Glossy black car paint — glossy, slightly lighter base
    Reflective *mat_carpaint = new Reflective();
    mat_carpaint->set_ka(0.15f);
    mat_carpaint->set_kd(0.15f);
    mat_carpaint->set_ks(0.9f);
    mat_carpaint->set_kr(0.6f); // stronger reflection
    mat_carpaint->set_exp(300.0f);
    mat_carpaint->set_cd(RGBColor(0.06f, 0.06f, 0.07f));
    mat_carpaint->set_cr(RGBColor(0.6f, 0.7f, 0.9f)); // blue-tinted reflection

    // 2. Red paint — glossy red
    Phong *mat_red = new Phong();
    mat_red->set_ka(0.12f);
    mat_red->set_kd(0.6f);
    mat_red->set_ks(0.4f);
    mat_red->set_exp(100.0f);
    mat_red->set_cd(RGBColor(0.8f, 0.015f, 0.012f));
    mat_red->set_cs(RGBColor(1.0f, 0.9f, 0.9f));

    // 3. Chrome / bright metal — highly reflective
    Reflective *mat_chrome = new Reflective();
    mat_chrome->set_ka(0.1f);
    mat_chrome->set_kd(0.3f);
    mat_chrome->set_ks(0.5f);
    mat_chrome->set_kr(0.4f);
    mat_chrome->set_exp(300.0f);
    mat_chrome->set_cd(RGBColor(0.53f, 0.53f, 0.53f));
    mat_chrome->set_cr(RGBColor(0.8f, 0.8f, 0.85f));

    // 4. Steel / dark metal — grey metallic
    Phong *mat_steel = new Phong();
    mat_steel->set_ka(0.15f);
    mat_steel->set_kd(0.45f);
    mat_steel->set_ks(0.45f);
    mat_steel->set_exp(120.0f);
    mat_steel->set_cd(RGBColor(0.35f, 0.35f, 0.35f));
    mat_steel->set_cs(RGBColor(0.7f, 0.7f, 0.75f));

    // 5. Concrete / building walls — medium grey
    Phong *mat_concrete = new Phong();
    mat_concrete->set_ka(0.2f);
    mat_concrete->set_kd(0.65f);
    mat_concrete->set_ks(0.1f);
    mat_concrete->set_exp(20.0f);
    mat_concrete->set_cd(RGBColor(0.4f, 0.4f, 0.38f));
    mat_concrete->set_cs(RGBColor(0.5f, 0.5f, 0.5f));

    // 6. House / canopy structure — original dark values restored
    Phong *mat_house = new Phong();
    mat_house->set_ka(0.25f);
    mat_house->set_kd(0.55f);
    mat_house->set_ks(0.2f);
    mat_house->set_exp(30.0f);
    mat_house->set_cd(RGBColor(0.35f, 0.35f, 0.35f));
    mat_house->set_cs(RGBColor(0.5f, 0.5f, 0.5f));

    // 7. Rubber / tyre — very dark matte
    Matte *mat_rubber = new Matte();
    mat_rubber->set_ka(0.08f);
    mat_rubber->set_kd(0.3f);
    mat_rubber->set_cd(RGBColor(0.03f, 0.03f, 0.03f));

    // 8. Glass / windows — bright reflective
    Reflective *mat_glass = new Reflective();
    mat_glass->set_ka(0.05f);
    mat_glass->set_kd(0.15f);
    mat_glass->set_ks(0.5f);
    mat_glass->set_kr(0.45f);
    mat_glass->set_exp(400.0f);
    mat_glass->set_cd(RGBColor(0.1f, 0.12f, 0.15f));
    mat_glass->set_cr(RGBColor(0.6f, 0.65f, 0.75f));

    // 9. White painted metal
    Phong *mat_white = new Phong();
    mat_white->set_ka(0.15f);
    mat_white->set_kd(0.65f);
    mat_white->set_ks(0.3f);
    mat_white->set_exp(80.0f);
    mat_white->set_cd(RGBColor(1.0f, 0.84f, 0.78f));
    mat_white->set_cs(RGBColor(1.0f, 0.95f, 0.9f));

    // 10. Road / asphalt — medium-dark matte
    Matte *mat_road = new Matte();
    mat_road->set_ka(0.12f);
    mat_road->set_kd(0.45f);
    mat_road->set_cd(RGBColor(0.2f, 0.2f, 0.2f));

    // 11. Green vegetation/plastic
    Phong *mat_green = new Phong();
    mat_green->set_ka(0.1f);
    mat_green->set_kd(0.5f);
    mat_green->set_ks(0.15f);
    mat_green->set_exp(30.0f);
    mat_green->set_cd(RGBColor(0.07f, 0.36f, 0.09f));
    mat_green->set_cs(RGBColor(0.5f, 0.6f, 0.5f));

    // 12. Emissive canopy lights — bright warm self-illuminated
    Phong *mat_emissive = new Phong();
    mat_emissive->set_ka(0.85f);
    mat_emissive->set_kd(0.5f);
    mat_emissive->set_ks(0.2f);
    mat_emissive->set_exp(10.0f);
    mat_emissive->set_cd(RGBColor(1.0f, 0.95f, 0.8f));
    mat_emissive->set_cs(RGBColor(1.0f, 1.0f, 1.0f));

    // 12b. Car headlight LED tubes — bright cool white
    Phong *mat_headlight = new Phong();
    mat_headlight->set_ka(1.0f);
    mat_headlight->set_kd(0.6f);
    mat_headlight->set_ks(0.3f);
    mat_headlight->set_exp(20.0f);
    mat_headlight->set_cd(RGBColor(1.0f, 1.0f, 1.0f));
    mat_headlight->set_cs(RGBColor(1.0f, 1.0f, 1.0f));

    // 12c. Headlight glass — mostly transparent-looking, bright
    Phong *mat_headlight_glass = new Phong();
    mat_headlight_glass->set_ka(0.3f);
    mat_headlight_glass->set_kd(0.3f);
    mat_headlight_glass->set_ks(0.5f);
    mat_headlight_glass->set_exp(300.0f);
    mat_headlight_glass->set_cd(RGBColor(1.2f, 1.2f, 1.2f)); // v bright for bloom (neon effect)
    mat_headlight_glass->set_cs(RGBColor(1.0f, 1.0f, 1.0f));

    // 13. Black plastic — dark grey, medium gloss
    Phong *mat_plastic = new Phong();
    mat_plastic->set_ka(0.15f);
    mat_plastic->set_kd(0.35f);
    mat_plastic->set_ks(0.4f);
    mat_plastic->set_exp(60.0f);
    mat_plastic->set_cd(RGBColor(0.12f, 0.12f, 0.12f));
    mat_plastic->set_cs(RGBColor(0.5f, 0.5f, 0.5f));

    // 14. Grey tiles — lighter concrete
    Phong *mat_tiles = new Phong();
    mat_tiles->set_ka(0.15f);
    mat_tiles->set_kd(0.6f);
    mat_tiles->set_ks(0.15f);
    mat_tiles->set_exp(30.0f);
    mat_tiles->set_cd(RGBColor(0.45f, 0.45f, 0.45f));
    mat_tiles->set_cs(RGBColor(0.6f, 0.6f, 0.6f));

    // 15. Orange indicators
    Phong *mat_indicator = new Phong();
    mat_indicator->set_ka(0.5f);
    mat_indicator->set_kd(0.6f);
    mat_indicator->set_ks(0.2f);
    mat_indicator->set_exp(30.0f);
    mat_indicator->set_cd(RGBColor(0.8f, 0.43f, 0.0f));
    mat_indicator->set_cs(RGBColor(1.0f, 0.8f, 0.5f));

    // 16. Red tail lights — emissive red
    Phong *mat_taillight = new Phong();
    mat_taillight->set_ka(0.6f);
    mat_taillight->set_kd(0.5f);
    mat_taillight->set_ks(0.3f);
    mat_taillight->set_exp(40.0f);
    mat_taillight->set_cd(RGBColor(0.8f, 0.05f, 0.02f));
    mat_taillight->set_cs(RGBColor(1.0f, 0.5f, 0.5f));

    // 17. Brown wood
    Matte *mat_wood = new Matte();
    mat_wood->set_ka(0.12f);
    mat_wood->set_kd(0.5f);
    mat_wood->set_cd(RGBColor(0.35f, 0.22f, 0.1f));

    // 18. Carbon fiber — medium-dark grey, highly specular
    Phong *mat_carbon = new Phong();
    mat_carbon->set_ka(0.15f);
    mat_carbon->set_kd(0.4f);
    mat_carbon->set_ks(0.6f);
    mat_carbon->set_exp(120.0f);
    mat_carbon->set_cd(RGBColor(0.2f, 0.2f, 0.2f));
    mat_carbon->set_cs(RGBColor(0.6f, 0.6f, 0.6f));

    // Pink for pillar stripes — will glow with bloom
    Phong *mat_pillar_pink = new Phong();
    mat_pillar_pink->set_ka(8.0f);
    mat_pillar_pink->set_kd(20.0f);
    mat_pillar_pink->set_ks(0.3f);
    mat_pillar_pink->set_exp(50.0f);
    mat_pillar_pink->set_cd(RGBColor(1.5f, 0.0f, 0.65f)); // bright pink
    mat_pillar_pink->set_cs(RGBColor(1.0f, 0.5f, 0.8f));

    // Dedicated emissive material JUST for the roof strips
    Phong *mat_roof_neon = new Phong();
    mat_roof_neon->set_ka(1.0f);
    mat_roof_neon->set_kd(0.5f); // allow direct light contribution too
    mat_roof_neon->set_ks(0.0f);
    mat_roof_neon->set_exp(1.0f);
    mat_roof_neon->set_cd(RGBColor(1.0f, 0.0f, 0.55f));
    mat_roof_neon->set_cs(RGBColor(0.0f, 0.0f, 0.0f));

    std::map<std::string, Material *> mtl_map;

    // sign colors
    mtl_map["pink"] = mat_roof_neon;
    mtl_map["pink.001"] = mat_roof_neon;
    mtl_map["blue"] = mat_roof_neon;
    mtl_map["blue.001"] = mat_roof_neon;
    mtl_map["orange"] = mat_indicator;
    mtl_map["yellow"] = mat_indicator;
    mtl_map["purple"] = mat_pillar_pink;
    mtl_map["red"] = mat_red;
    mtl_map["green"] = mat_green;

    // Glossy black car paint
    mtl_map["CarPaint"] = mat_carpaint;
    mtl_map["Black_Carpaint"] = mat_bike_frame;
    mtl_map["glossy_black_metal"] = mat_bike_frame;
    mtl_map["brembo"] = mat_carpaint;
    mtl_map["rims"] = mat_bike_engine;

    // Red
    mtl_map["RED"] = mat_pillar_pink;
    mtl_map["Roof_Neon"] = mat_roof_neon;
    mtl_map["Material.001"] = mat_red;
    mtl_map["red_metal"] = mat_red;
    mtl_map["brakes"] = mat_red;
    mtl_map["red_clipper"] = mat_red;

    // Chrome / bright metal
    mtl_map["chrome"] = mat_chrome;
    mtl_map["Metal"] = mat_chrome;
    mtl_map["chain"] = mat_charcoal;
    mtl_map["disk_brake"] = mat_bike_engine;
    mtl_map["emblem"] = mat_chrome;
    mtl_map["engine"] = mat_bike_engine;
    mtl_map["Brushed_used_metal_silver"] = mat_chrome;

    // Steel / darker metal
    mtl_map["STEEL"] = mat_bike_engine;
    mtl_map["darker_metal"] = mat_bike_frame;
    mtl_map["shock_absorbers"] = mat_bike_body;
    mtl_map["gril"] = mat_steel;
    mtl_map["gril_black"] = mat_steel;
    mtl_map["black_metal"] = mat_bike_frame;
    mtl_map["black_button"] = mat_steel;

    // Concrete / building
    mtl_map["Concrete_smooth"] = mat_concrete;
    mtl_map["Concrete_Slabs"] = mat_concrete;

    // House exterior
    mtl_map["HOUSE"] = mat_house;
    mtl_map["HOUSE_+_TRASH.001"] = mat_house;

    // Rubber / tyres
    mtl_map["RUBBER"] = mat_rubber;
    mtl_map["tyre"] = mat_rubber;
    mtl_map["tyre_side_wall"] = mat_rubber;
    mtl_map["Black_rubber"] = mat_rubber;
    mtl_map["leather"] = mat_rubber;

    // Glass / windows
    mtl_map["GLASS"] = mat_glass;
    mtl_map["WINDOWS"] = mat_glass;
    mtl_map["mirror"] = mat_glass;
    mtl_map["wind"] = mat_glass;

    // White painted
    mtl_map["Rusty_painted_metal_white"] = mat_white;

    // Road / asphalt
    mtl_map["Full_Road_Texture"] = mat_road;
    mtl_map["asphalt_cracked"] = mat_road;

    // Green
    mtl_map["Green"] = mat_green;
    mtl_map["green_metal"] = mat_green;
    mtl_map["matte_plastic_"] = mat_green;
    mtl_map["Grass_moss"] = mat_green;
    mtl_map["Stylized_Brown_Grass_and_Stone"] = mat_green;

    // Canopy emissive light surfaces
    mtl_map["light"] = mat_emissive;
    mtl_map["Automotive_led_lights.HOUSE"] = mat_emissive;

    // Car headlight LED tubes — bright white
    mtl_map["Head_Light_Led_Tube"] = mat_headlight;
    mtl_map["Head_Light_Led_Tube.001"] = mat_headlight;

    // Headlight glass — rendered as glowing since we can't do transparency
    mtl_map["HeadLight_Glass"] = mat_headlight;

    // tried transparency class but did not work out

    // Transparent* mat_hlight_glass = new Transparent(0.08f, RGBColor(0.9f, 0.95f, 1.0f));
    // mtl_map["HeadLight_Glass"] = mat_hlight_glass;

    // Tail lights — red emissive
    mtl_map["Tailight_Led_Tube"] = mat_taillight;
    mtl_map["Tailight_Led_Tube.001"] = mat_taillight;
    mtl_map["taillight"] = mat_taillight;

    // Orange indicators
    mtl_map["indicatores"] = mat_indicator;

    // Black plastic
    mtl_map["plastic"] = mat_plastic;
    mtl_map["plastic.001"] = mat_plastic;

    // Grey tiles
    mtl_map["Grey_tiles"] = mat_tiles;

    // Wood
    mtl_map["Wood_Planks_with_Nails"] = mat_wood;

    // Carbon fiber
    mtl_map["Carbon_fiber"] = mat_carbon;
    mtl_map["Carbon_Fiber_PBR_Interior.001"] = mat_carbon;

    // Gas pump body — red to match Blender reference
    mtl_map["GAS"] = mat_red;
    mtl_map["containers"] = mat_red;

    // Textured materials w/o textures — use default
    mtl_map["CHECK_YOUR_OIL"] = mat_default;
    mtl_map["bmw_logo"] = mat_default;
    mtl_map["engine_sticker"] = mat_default;
    mtl_map["Container_sheet_rusty"] = mat_default;

    // Ground
    Reflective *gnd_mat = new Reflective();
    gnd_mat->set_ka(0.15f);
    gnd_mat->set_kd(0.4f);
    gnd_mat->set_ks(0.4f);
    gnd_mat->set_kr(0.2f);
    gnd_mat->set_exp(60.0f);
    gnd_mat->set_cd(RGBColor(0.12f, 0.12f, 0.13f));
    gnd_mat->set_cr(RGBColor(0.5f, 0.55f, 0.7f));

    Plane *ground = new Plane(
        Point3D(0.0f, -7.69f, 0.0f),
        Vector3D(0.0f, 1.0f, 0.0f));
    ground->set_material(gnd_mat);
    add_geometry(ground);

    OBJTransform t(1.0f, 7.96f, -7.67f, 0.0f);
    load_obj_mtl("Scene/GasStation_Car_Bike.obj", *this, mtl_map, mat_default, t);
}
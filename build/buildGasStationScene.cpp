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
#include <map>
#include <string>

void World::build(void)
{
    // Scene bounds (with transform 7.96, -7.67, 0):
    //   min: (-13.92, -7.69, -18.66)  max: (9.09, -2.99, 2.42)
    //   center: (-2.42, -5.34, -8.12) size: (23.01, 4.70, 21.08)
    //
    // Blender→OBJ axis: X→X, Z→Y, Y→-Z, then +OBJTransform
    // Camera: Blender(-6.70, -6.61, 1.83) → OBJ(1.26, -5.84, 6.61)

    // VP centered on camera X,Y for straight-ahead view (no lateral skew).
    // Camera at (1.26, -5.84, 6.61), VP at Z=2.
    // VP center = (1.26, -5.84). Size 7.0 x 5.25 (4:3).
    vplane.top_left.x     = -2.24f;
    vplane.top_left.y     = -3.22f;
    vplane.top_left.z     =  2.0f;
    vplane.bottom_right.x =  4.76f;
    vplane.bottom_right.y = -8.47f;
    vplane.bottom_right.z =  2.0f;
    vplane.hres = 1920;
    vplane.vres = 1080;

    bg_color          = RGBColor(0.0f, 0.0f, 0.0f);
    ambient_color     = RGBColor(1.0f, 0.95f, 0.85f);
    ambient_intensity = 0.45f;

    set_camera(new Perspective(1.26f, -5.84f, 6.61f));
    sampler_ptr = new Jittered(camera_ptr, &vplane, 8);
    set_tracer(new ShadowCaster(this));
    set_acceleration(new BVH(this));

    // -------------------------------------------------------
    // LIGHTS — 8 canopy lights from Blender Light Positions.txt
    // Blender (Bx,By,Bz) → OBJ (Bx+7.96, Bz-7.67, -By)
    // All at OBJ Y=-3.37, two X columns, four Z rows
    // -------------------------------------------------------
    RGBColor warm(1.0f, 0.92f, 0.75f);
    float canopy_i = 250.0f;

    // Canopy lights at Y=-3.60 (below ceiling) for clear downward illumination
    add_light(new PointLight(Point3D(-1.29f, -3.60f, -10.28f), warm, canopy_i));
    add_light(new PointLight(Point3D( 1.29f, -3.60f, -10.28f), warm, canopy_i));
    add_light(new PointLight(Point3D( 1.29f, -3.60f,  -8.54f), warm, canopy_i));
    add_light(new PointLight(Point3D( 1.29f, -3.60f,  -5.06f), warm, canopy_i));
    add_light(new PointLight(Point3D(-1.29f, -3.60f,  -8.54f), warm, canopy_i));
    add_light(new PointLight(Point3D(-1.28f, -3.60f,  -5.06f), warm, canopy_i));
    add_light(new PointLight(Point3D(-1.29f, -3.60f,  -6.80f), warm, canopy_i));
    add_light(new PointLight(Point3D( 1.29f, -3.60f,  -6.80f), warm, canopy_i));

    // Broad fill lights — station left wing and right side
    add_light(new PointLight(Point3D(-8.0f, -3.6f,  -7.0f), warm, 120.0f));
    add_light(new PointLight(Point3D(-8.0f, -3.6f, -12.0f), warm, 120.0f));
    add_light(new PointLight(Point3D( 5.0f, -3.6f,  -7.0f), warm, 80.0f));
    add_light(new PointLight(Point3D( 5.0f, -3.6f, -12.0f), warm, 80.0f));

    // Extra fill lights near ground to brighten under-canopy floor
    add_light(new PointLight(Point3D( 0.0f, -6.5f,  -7.5f), warm, 40.0f));
    add_light(new PointLight(Point3D( 0.0f, -6.5f, -10.0f), warm, 40.0f));
    add_light(new PointLight(Point3D(-4.0f, -6.5f,  -8.0f), warm, 30.0f));

    // Strong directional downward — canopy ceiling glow
    add_light(new DirectionalLight(
        Vector3D(0.0f, -1.0f, 0.0f),
        RGBColor(1.0f, 0.92f, 0.75f), 3.0f));

    // Blue moonlight from behind and slightly above camera
    add_light(new DirectionalLight(
        Vector3D(0.0f, -0.3f, -1.0f),
        RGBColor(0.12f, 0.14f, 0.28f), 0.6f));

    // -------------------------------------------------------
    // MATERIALS — mapped to usemtl names from MTL file
    // -------------------------------------------------------

    // Default fallback for unmapped materials
    Phong* mat_default = new Phong();
    mat_default->set_ka(0.15f);
    mat_default->set_kd(0.55f);
    mat_default->set_ks(0.3f);
    mat_default->set_exp(50.0f);
    mat_default->set_cd(RGBColor(0.5f, 0.48f, 0.45f));
    mat_default->set_cs(RGBColor(1.0f, 0.95f, 0.85f));

    // 1. Glossy black car paint — dark reflective
    Reflective* mat_carpaint = new Reflective();
    mat_carpaint->set_ka(0.05f);
    mat_carpaint->set_kd(0.08f);
    mat_carpaint->set_ks(0.6f);
    mat_carpaint->set_kr(0.35f);
    mat_carpaint->set_exp(200.0f);
    mat_carpaint->set_cd(RGBColor(0.02f, 0.02f, 0.02f));
    mat_carpaint->set_cr(RGBColor(0.4f, 0.4f, 0.45f));

    // 2. Red paint — glossy red
    Phong* mat_red = new Phong();
    mat_red->set_ka(0.12f);
    mat_red->set_kd(0.6f);
    mat_red->set_ks(0.4f);
    mat_red->set_exp(100.0f);
    mat_red->set_cd(RGBColor(0.8f, 0.015f, 0.012f));
    mat_red->set_cs(RGBColor(1.0f, 0.9f, 0.9f));

    // 3. Chrome / bright metal — highly reflective
    Reflective* mat_chrome = new Reflective();
    mat_chrome->set_ka(0.1f);
    mat_chrome->set_kd(0.3f);
    mat_chrome->set_ks(0.5f);
    mat_chrome->set_kr(0.4f);
    mat_chrome->set_exp(300.0f);
    mat_chrome->set_cd(RGBColor(0.53f, 0.53f, 0.53f));
    mat_chrome->set_cr(RGBColor(0.8f, 0.8f, 0.85f));

    // 4. Steel / dark metal — grey metallic
    Phong* mat_steel = new Phong();
    mat_steel->set_ka(0.15f);
    mat_steel->set_kd(0.45f);
    mat_steel->set_ks(0.45f);
    mat_steel->set_exp(120.0f);
    mat_steel->set_cd(RGBColor(0.35f, 0.35f, 0.35f));
    mat_steel->set_cs(RGBColor(0.7f, 0.7f, 0.75f));

    // 5. Concrete / building walls — medium grey
    Phong* mat_concrete = new Phong();
    mat_concrete->set_ka(0.2f);
    mat_concrete->set_kd(0.65f);
    mat_concrete->set_ks(0.1f);
    mat_concrete->set_exp(20.0f);
    mat_concrete->set_cd(RGBColor(0.4f, 0.4f, 0.38f));
    mat_concrete->set_cs(RGBColor(0.5f, 0.5f, 0.5f));

    // 6. House / canopy structure — lighter grey so top edge is visible against black sky
    Phong* mat_house = new Phong();
    mat_house->set_ka(0.25f);
    mat_house->set_kd(0.55f);
    mat_house->set_ks(0.2f);
    mat_house->set_exp(30.0f);
    mat_house->set_cd(RGBColor(0.35f, 0.35f, 0.35f));
    mat_house->set_cs(RGBColor(0.5f, 0.5f, 0.5f));

    // 7. Rubber / tyre — very dark matte
    Matte* mat_rubber = new Matte();
    mat_rubber->set_ka(0.08f);
    mat_rubber->set_kd(0.3f);
    mat_rubber->set_cd(RGBColor(0.03f, 0.03f, 0.03f));

    // 8. Glass / windows — bright reflective
    Reflective* mat_glass = new Reflective();
    mat_glass->set_ka(0.05f);
    mat_glass->set_kd(0.15f);
    mat_glass->set_ks(0.5f);
    mat_glass->set_kr(0.45f);
    mat_glass->set_exp(400.0f);
    mat_glass->set_cd(RGBColor(0.1f, 0.12f, 0.15f));
    mat_glass->set_cr(RGBColor(0.6f, 0.65f, 0.75f));

    // 9. White painted metal
    Phong* mat_white = new Phong();
    mat_white->set_ka(0.15f);
    mat_white->set_kd(0.65f);
    mat_white->set_ks(0.3f);
    mat_white->set_exp(80.0f);
    mat_white->set_cd(RGBColor(1.0f, 0.84f, 0.78f));
    mat_white->set_cs(RGBColor(1.0f, 0.95f, 0.9f));

    // 10. Road / asphalt — medium-dark matte
    Matte* mat_road = new Matte();
    mat_road->set_ka(0.12f);
    mat_road->set_kd(0.45f);
    mat_road->set_cd(RGBColor(0.2f, 0.2f, 0.2f));

    // 11. Green vegetation/plastic
    Phong* mat_green = new Phong();
    mat_green->set_ka(0.1f);
    mat_green->set_kd(0.5f);
    mat_green->set_ks(0.15f);
    mat_green->set_exp(30.0f);
    mat_green->set_cd(RGBColor(0.07f, 0.36f, 0.09f));
    mat_green->set_cs(RGBColor(0.5f, 0.6f, 0.5f));

    // 12. Emissive canopy lights — bright warm self-illuminated
    Phong* mat_emissive = new Phong();
    mat_emissive->set_ka(0.85f);
    mat_emissive->set_kd(0.5f);
    mat_emissive->set_ks(0.2f);
    mat_emissive->set_exp(10.0f);
    mat_emissive->set_cd(RGBColor(1.0f, 0.95f, 0.8f));
    mat_emissive->set_cs(RGBColor(1.0f, 1.0f, 1.0f));

    // 12b. Car headlight LED tubes — bright cool white
    Phong* mat_headlight = new Phong();
    mat_headlight->set_ka(1.0f);
    mat_headlight->set_kd(0.6f);
    mat_headlight->set_ks(0.3f);
    mat_headlight->set_exp(20.0f);
    mat_headlight->set_cd(RGBColor(1.0f, 1.0f, 1.0f));
    mat_headlight->set_cs(RGBColor(1.0f, 1.0f, 1.0f));

    // 12c. Headlight glass — mostly transparent-looking, bright
    Phong* mat_headlight_glass = new Phong();
    mat_headlight_glass->set_ka(0.3f);
    mat_headlight_glass->set_kd(0.3f);
    mat_headlight_glass->set_ks(0.5f);
    mat_headlight_glass->set_exp(300.0f);
    mat_headlight_glass->set_cd(RGBColor(0.7f, 0.75f, 0.8f));
    mat_headlight_glass->set_cs(RGBColor(1.0f, 1.0f, 1.0f));

    // 13. Black plastic — dark low-specular
    Phong* mat_plastic = new Phong();
    mat_plastic->set_ka(0.05f);
    mat_plastic->set_kd(0.15f);
    mat_plastic->set_ks(0.3f);
    mat_plastic->set_exp(150.0f);
    mat_plastic->set_cd(RGBColor(0.01f, 0.01f, 0.01f));
    mat_plastic->set_cs(RGBColor(0.4f, 0.4f, 0.4f));

    // 14. Grey tiles — lighter concrete
    Phong* mat_tiles = new Phong();
    mat_tiles->set_ka(0.15f);
    mat_tiles->set_kd(0.6f);
    mat_tiles->set_ks(0.15f);
    mat_tiles->set_exp(30.0f);
    mat_tiles->set_cd(RGBColor(0.45f, 0.45f, 0.45f));
    mat_tiles->set_cs(RGBColor(0.6f, 0.6f, 0.6f));

    // 15. Orange indicators
    Phong* mat_indicator = new Phong();
    mat_indicator->set_ka(0.5f);
    mat_indicator->set_kd(0.6f);
    mat_indicator->set_ks(0.2f);
    mat_indicator->set_exp(30.0f);
    mat_indicator->set_cd(RGBColor(0.8f, 0.43f, 0.0f));
    mat_indicator->set_cs(RGBColor(1.0f, 0.8f, 0.5f));

    // 16. Red tail lights — emissive red
    Phong* mat_taillight = new Phong();
    mat_taillight->set_ka(0.6f);
    mat_taillight->set_kd(0.5f);
    mat_taillight->set_ks(0.3f);
    mat_taillight->set_exp(40.0f);
    mat_taillight->set_cd(RGBColor(0.8f, 0.05f, 0.02f));
    mat_taillight->set_cs(RGBColor(1.0f, 0.5f, 0.5f));

    // 17. Brown wood
    Matte* mat_wood = new Matte();
    mat_wood->set_ka(0.12f);
    mat_wood->set_kd(0.5f);
    mat_wood->set_cd(RGBColor(0.35f, 0.22f, 0.1f));

    // 18. Carbon fiber — dark grey
    Phong* mat_carbon = new Phong();
    mat_carbon->set_ka(0.08f);
    mat_carbon->set_kd(0.3f);
    mat_carbon->set_ks(0.4f);
    mat_carbon->set_exp(200.0f);
    mat_carbon->set_cd(RGBColor(0.08f, 0.08f, 0.08f));
    mat_carbon->set_cs(RGBColor(0.5f, 0.5f, 0.5f));

    // Build material map — every usemtl name from the MTL file
    std::map<std::string, Material*> mtl_map;

    // Glossy black car paint
    mtl_map["CarPaint"]           = mat_carpaint;
    mtl_map["Black_Carpaint"]     = mat_carpaint;
    mtl_map["glossy_black_metal"] = mat_carpaint;
    mtl_map["brembo"]             = mat_carpaint;
    mtl_map["rims"]               = mat_carpaint;

    // Red
    mtl_map["RED"]                = mat_red;
    mtl_map["Material.001"]       = mat_red;
    mtl_map["red_metal"]          = mat_red;
    mtl_map["brakes"]             = mat_red;
    mtl_map["red_clipper"]        = mat_red;

    // Chrome / bright metal
    mtl_map["chrome"]             = mat_chrome;
    mtl_map["Metal"]              = mat_chrome;
    mtl_map["chain"]              = mat_chrome;
    mtl_map["disk_brake"]         = mat_chrome;
    mtl_map["emblem"]             = mat_chrome;
    mtl_map["engine"]             = mat_chrome;
    mtl_map["Brushed_used_metal_silver"] = mat_chrome;

    // Steel / darker metal
    mtl_map["STEEL"]              = mat_steel;
    mtl_map["darker_metal"]       = mat_steel;
    mtl_map["shock_absorbers"]    = mat_steel;
    mtl_map["gril"]               = mat_steel;
    mtl_map["gril_black"]         = mat_steel;
    mtl_map["black_metal"]        = mat_steel;
    mtl_map["black_button"]       = mat_steel;

    // Concrete / building
    mtl_map["Concrete_smooth"]    = mat_concrete;
    mtl_map["Concrete_Slabs"]     = mat_concrete;

    // House exterior
    mtl_map["HOUSE"]              = mat_house;
    mtl_map["HOUSE_+_TRASH.001"]  = mat_house;

    // Rubber / tyres
    mtl_map["RUBBER"]             = mat_rubber;
    mtl_map["tyre"]               = mat_rubber;
    mtl_map["tyre_side_wall"]     = mat_rubber;
    mtl_map["Black_rubber"]       = mat_rubber;
    mtl_map["leather"]            = mat_rubber;

    // Glass / windows
    mtl_map["GLASS"]              = mat_glass;
    mtl_map["WINDOWS"]            = mat_glass;
    mtl_map["mirror"]             = mat_glass;
    mtl_map["wind"]               = mat_glass;

    // White painted
    mtl_map["Rusty_painted_metal_white"] = mat_white;

    // Road / asphalt
    mtl_map["Full_Road_Texture"]  = mat_road;
    mtl_map["asphalt_cracked"]    = mat_road;

    // Green
    mtl_map["Green"]              = mat_green;
    mtl_map["green_metal"]        = mat_green;
    mtl_map["matte_plastic_"]     = mat_green;
    mtl_map["Grass_moss"]         = mat_green;
    mtl_map["Stylized_Brown_Grass_and_Stone"] = mat_green;

    // Canopy emissive light surfaces
    mtl_map["light"]                      = mat_emissive;
    mtl_map["Automotive_led_lights.HOUSE"] = mat_emissive;

    // Car headlight LED tubes — bright white
    mtl_map["Head_Light_Led_Tube"]        = mat_headlight;
    mtl_map["Head_Light_Led_Tube.001"]    = mat_headlight;

    // Headlight glass — rendered as glowing since we can't do transparency
    mtl_map["HeadLight_Glass"]            = mat_headlight;

    // Tail lights — red emissive
    mtl_map["Tailight_Led_Tube"]     = mat_taillight;
    mtl_map["Tailight_Led_Tube.001"] = mat_taillight;
    mtl_map["taillight"]             = mat_taillight;

    // Orange indicators
    mtl_map["indicatores"]        = mat_indicator;

    // Black plastic
    mtl_map["plastic"]            = mat_plastic;
    mtl_map["plastic.001"]        = mat_plastic;

    // Grey tiles
    mtl_map["Grey_tiles"]         = mat_tiles;

    // Wood
    mtl_map["Wood_Planks_with_Nails"] = mat_wood;

    // Carbon fiber
    mtl_map["Carbon_fiber"]                  = mat_carbon;
    mtl_map["Carbon_Fiber_PBR_Interior.001"] = mat_carbon;

    // Gas pump body — red to match Blender reference
    mtl_map["GAS"]                = mat_red;
    mtl_map["containers"]         = mat_red;

    // Textured materials w/o textures — use default
    mtl_map["CHECK_YOUR_OIL"]     = mat_default;
    mtl_map["bmw_logo"]           = mat_default;
    mtl_map["engine_sticker"]     = mat_default;
    mtl_map["Container_sheet_rusty"] = mat_default;

    // Ground plane — dark reflective asphalt
    Reflective* gnd_mat = new Reflective();
    gnd_mat->set_ka(0.1f);
    gnd_mat->set_kd(0.35f);
    gnd_mat->set_ks(0.4f);
    gnd_mat->set_kr(0.2f);
    gnd_mat->set_exp(60.0f);
    gnd_mat->set_cd(RGBColor(0.15f, 0.15f, 0.17f));
    gnd_mat->set_cr(RGBColor(0.5f, 0.55f, 0.7f));

    Plane* ground = new Plane(
        Point3D(0.0f, -7.69f, 0.0f),
        Vector3D(0.0f, 1.0f, 0.0f));
    ground->set_material(gnd_mat);
    add_geometry(ground);

    OBJTransform t(1.0f, 7.96f, -7.67f, 0.0f);
    load_obj_mtl("Scene/GasStation_Car_Bike.obj", *this, mtl_map, mat_default, t);
}

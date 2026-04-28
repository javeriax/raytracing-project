#include "World.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/Constants.hpp"
#include "../geometry/Geometry.hpp"
#include "../cameras/Camera.hpp"
#include "../samplers/Sampler.hpp"
#include "../lights/Light.hpp"
#include "../tracers/Tracer.hpp"
#include "../acceleration/Acceleration.hpp"

World::World()
    : bg_color(0),
      camera_ptr(nullptr),
      sampler_ptr(nullptr),
      tracer_ptr(nullptr),
      acceleration_ptr(nullptr),
      ambient_color(1, 1, 1),
      ambient_intensity(0.1f) {}

World::~World() {
    delete camera_ptr;
    delete sampler_ptr;
    delete tracer_ptr;
    delete acceleration_ptr;
    for (auto g : geometry) delete g;
    for (auto l : lights)   delete l;
}

void World::add_geometry(Geometry* geom_ptr) {
    geometry.push_back(geom_ptr);
}

void World::add_light(Light* light_ptr) {
    lights.push_back(light_ptr);
}

void World::set_camera(Camera* c_ptr) {
    camera_ptr = c_ptr;
}

void World::set_tracer(Tracer* t_ptr) {
    tracer_ptr = t_ptr;
}

void World::set_acceleration(Acceleration* a_ptr) {
    acceleration_ptr = a_ptr;
}

// finds the closest object the ray hits
ShadeInfo World::hit_objects(const Ray& ray) {
    ShadeInfo sinfo(*this);
    float t_min = kHugeValue;
    float t;

    // if we have an acceleration structure use it
    if (acceleration_ptr) {
        return acceleration_ptr->hit(ray);
    }

    // otherwise brute force check every object
    for (auto g : geometry) {
        ShadeInfo si(*this);
        if (g->hit(ray, t, si) && t < t_min) {
            t_min = t;
            sinfo = si;
        }
    }
    return sinfo;
}

// checks if anything blocks the ray before reaching the light
bool World::in_shadow(const Ray& ray, float max_t) const {
    float t;

    if (acceleration_ptr) {
        return acceleration_ptr->shadow_hit(ray, max_t);
    }

    for (auto g : geometry) {
        ShadeInfo si(*this);
        if (g->hit(ray, t, si) && t < max_t && t > kEpsilon) {
            return true;
        }
    }
    return false;
}

// tells acceleration structure to build itself from current geometry
void World::build_acceleration() {
    if (acceleration_ptr) {
        acceleration_ptr->build(geometry);
    }
}

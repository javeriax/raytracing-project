#pragma once

#include "../utilities/RGBColor.hpp"
#include "../utilities/Ray.hpp"

class World;

// abstract base class for all tracers
// a tracer decides what color a ray produces
class Tracer {
protected:
    World* world_ptr;   // access to scene geometry, lights, etc

public:
    Tracer(World* w);
    virtual ~Tracer() = default;

    // trace a ray and return the color it produces
    // depth tracks recursion level to stop infinite reflections
    virtual RGBColor trace_ray(const Ray& ray, int depth = 0) const = 0;
};

#pragma once

#include <vector>
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Ray.hpp"

class Geometry;

// abstract base class for acceleration structures
class Acceleration
{
public:
    Acceleration() = default;
    virtual ~Acceleration() = default;

    // build the structure from a list of geometry objects
    virtual void build(const std::vector<Geometry *> &objects) = 0;

    // find closest hit - returns shadeinfo
    virtual ShadeInfo hit(const Ray &ray) const = 0;

    // returns true if anything blocks the ray before max_t
    virtual bool shadow_hit(const Ray &ray, float max_t) const = 0;
};

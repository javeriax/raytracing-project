#pragma once

#include "Tracer.hpp"

// basic tracer - just finds the closest hit and calls shade()
// no shadows, no reflections - same as homework 5 raytracer.cpp
class Basic : public Tracer {
public:
    Basic(World* w);
    virtual RGBColor trace_ray(const Ray& ray, int depth = 0) const override;
};

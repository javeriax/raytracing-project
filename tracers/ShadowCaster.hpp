#pragma once

#include "Tracer.hpp"

// shadow caster tracer - full lighting with shadow rays

class ShadowCaster : public Tracer
{
public:
    ShadowCaster(World *w);
    virtual RGBColor trace_ray(const Ray &ray, int depth = 0) const override;
};

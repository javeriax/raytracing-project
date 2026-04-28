#pragma once

#include "Tracer.hpp"

// shadow caster tracer - full lighting with shadow rays
// materials handle shadows themselves via world->in_shadow()
// this tracer just calls shade() which uses the lights properly
class ShadowCaster : public Tracer {
public:
    ShadowCaster(World* w);
    virtual RGBColor trace_ray(const Ray& ray, int depth = 0) const override;
};

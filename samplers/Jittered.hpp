#pragma once

#include "Sampler.hpp"

// jittered sampler - shoots n*n rays per pixel in a grid
// each ray is randomly offset within its grid cell
// this removes aliasing (jagged edges) by averaging multiple samples
// aliasing happens because simple sampler only shoots 1 ray dead center
class Jittered : public Sampler
{
private:
    int n; // grid is n x n, so n*n rays per pixel total

public:
    Jittered() = default;
    Jittered(Camera *c_ptr, ViewPlane *v_ptr, int grid_size = 4);

    Jittered(const Jittered &other);
    Jittered &operator=(const Jittered &other);

    virtual ~Jittered() = default;

    // returns n*n rays, each randomly offset within its subcell
    virtual std::vector<Ray> get_rays(int px, int py) const override;
};

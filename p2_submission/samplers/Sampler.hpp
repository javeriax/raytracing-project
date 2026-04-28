#pragma once
#include <vector>
class Camera; class Ray; class ViewPlane;
class Sampler {
protected:
  Camera *camera_ptr;
  ViewPlane *viewplane_ptr;
public:
  Sampler(); Sampler(Camera *c_ptr, ViewPlane *v_ptr);
  Sampler(const Sampler &camera) = default;
  Sampler &operator=(const Sampler &other) = default;
  virtual ~Sampler() = default;
  virtual std::vector<Ray> get_rays(int px, int py) const = 0;
};

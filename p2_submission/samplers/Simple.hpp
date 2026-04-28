#pragma once
#include "Sampler.hpp"
class Simple : public Sampler {
public:
  Simple() = default;
  Simple(Camera *c_ptr, ViewPlane *v_ptr);
  Simple(const Simple &camera); Simple &operator=(const Simple &other);
  virtual ~Simple() = default;
  std::vector<Ray> get_rays(int px, int py) const override;
};

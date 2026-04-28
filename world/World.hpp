#pragma once

/**
   This file declares the World class which contains all the information about
   the scene - geometry and materials, lights, viewplane, camera, samplers, and
   acceleration structures.

   It also traces rays through the scene.

   Courtesy Kevin Suffern.
*/

#include <vector>
#include "../utilities/RGBColor.hpp"
#include "ViewPlane.hpp"

class Camera;
class Geometry;
class Ray;
class Sampler;
class ShadeInfo;
class Light;
class Tracer;
class Acceleration;

class World
{
public:
  ViewPlane vplane;
  RGBColor bg_color;
  std::vector<Geometry *> geometry;
  std::vector<Light *> lights; // all lights in the scene
  Camera *camera_ptr;
  Sampler *sampler_ptr;
  Tracer *tracer_ptr;             // handles ray casting logic
  Acceleration *acceleration_ptr; // speeds up intersection tests
  RGBColor ambient_color;         // base ambient light color
  float ambient_intensity;        // how strong the ambient light is

public:
  // Constructors.
  World(); // initialize members.

  // Destructor.
  ~World(); // free memory.

  // Add to the scene.
  void add_geometry(Geometry *geom_ptr);
  void add_light(Light *light_ptr);
  void set_camera(Camera *c_ptr);
  void set_tracer(Tracer *t_ptr);
  void set_acceleration(Acceleration *a_ptr);

  // Build scene - add all geometry, materials, lights, viewplane, camera,
  // samplers, and acceleration structures
  void build();

  // Returns appropriate shading information corresponding to intersection of
  // the ray with the scene geometry.
  ShadeInfo hit_objects(const Ray &ray);

  // returns true if ray hits anything before distance max_t
  // used for shadow rays
  bool in_shadow(const Ray &ray, float max_t) const;

  // builds the acceleration structure from current geometry
  void build_acceleration();
};

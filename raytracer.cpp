#include <iostream>
#include "samplers/Sampler.hpp"
#include "image/Image.hpp"
#include "utilities/RGBColor.hpp"
#include "utilities/Ray.hpp"
#include "utilities/ShadeInfo.hpp"
#include "materials/Material.hpp"
#include "world/World.hpp"
#include "world/ViewPlane.hpp"
#include "tracers/Tracer.hpp"
#include "image/Bloom.hpp"   
#include <chrono>

#define USE_ACCELERATION true   // set to false to render without BVH

int main(int argc, char **argv)
{
  World world;
  world.build();

  // build acceleration structure after all geometry is added
  if (USE_ACCELERATION) {
    world.build_acceleration();  // builds and uses BVH
  } else {
    // acceleration_ptr stays nullptr — World::hit_objects brute forces all geometry
    std::cout << "Rendering WITHOUT acceleration structure (brute force)" << std::endl;
  }

  Sampler *sampler = world.sampler_ptr;
  ViewPlane &viewplane = world.vplane;
  Image image(viewplane);

  std::vector<Ray> rays;

  auto start = std::chrono::high_resolution_clock::now();
  for (int x = 0; x < viewplane.hres; x++)
  {
    for (int y = 0; y < viewplane.vres; y++)
    {
      RGBColor pixel_color(0);
      rays = sampler->get_rays(x, y);

      for (const auto &ray : rays)
      {
        // tracer decides how to shade each ray
        // basic tracer = no shadows
        // shadowcaster tracer = full shadows + reflections
        if (world.tracer_ptr)
        {
          pixel_color += ray.w * world.tracer_ptr->trace_ray(ray, 0);
        }
        else
        {
          // fallback if no tracer set
          ShadeInfo sinfo = world.hit_objects(ray);
          if (sinfo.hit)
            pixel_color += ray.w * sinfo.material_ptr->shade(sinfo);
          else
            pixel_color += ray.w * world.bg_color;
        }
      }
      image.set_pixel(x, y, pixel_color);
    }

    // progress indicator every 50 columns
    if (x % 50 == 0)
    {
      std::cout << "rendering: " << (x * 100 / viewplane.hres) << "%\r";
      std::cout.flush();
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  double seconds = std::chrono::duration<double>(end - start).count();
  std::cout << "\nRender time: " << (int)(seconds/3600) << "h "
            << (int)((int)seconds%3600/60) << "m "
            << (int)seconds%60 << "s\n";
    // Bloom parameters scale with resolution
  int bloom_radius;
  float bloom_threshold, bloom_strength;

  // if (world.vplane.hres >= 1920) {
  //     // High quality 1920x1080
  //     bloom_radius    = 30;
  //     bloom_threshold = 0.45f;
  //     bloom_strength  = 1.5f;
  // } else {
  //     // Low quality 480x360
  //     bloom_radius    = 14;
  //     bloom_threshold = 0.45f;
  //     bloom_strength  = 1.5f;
  // }

  if (world.vplane.hres >= 1920) {
      bloom_radius    = 30;
      bloom_threshold = 0.2f;   // changed from 0.45
      bloom_strength  = 2.0f;   // changed from 1.5
  } else {
      bloom_radius    = 14;
      bloom_threshold = 0.2f;   // changed from 0.45
      bloom_strength  = 2.0f;   // changed from 1.5
  }
  Bloom bloom(bloom_threshold, bloom_radius, bloom_strength);
  bloom.apply(image);
  image.write_png("scene.png");

  std::cout << "\nwrote scene.png\n";
  return 0;
}

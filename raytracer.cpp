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

int main(int argc, char **argv)
{
  World world;
  world.build();

  // build acceleration structure after all geometry is added
  world.build_acceleration();

  Sampler *sampler = world.sampler_ptr;
  ViewPlane &viewplane = world.vplane;
  Image image(viewplane);

  std::vector<Ray> rays;

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

  image.write_png("scene.png");
  std::cout << "\nwrote scene.png\n";
  return 0;
}

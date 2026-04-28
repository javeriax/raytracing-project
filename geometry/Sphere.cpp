#include "Sphere.hpp"
#include "../utilities/BBox.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Constants.hpp"
#include <cmath>
#include <sstream>

Sphere::Sphere() : c(0, 0, 0), r(0) {}

Sphere::Sphere(const Point3D &center, float radius) : c(center), r(radius) {}

Sphere::Sphere(const Sphere &obj) : Geometry(obj), c(obj.c), r(obj.r) {}

Sphere &Sphere::operator=(const Sphere &rhs) {
    if (this != &rhs) { Geometry::operator=(rhs); c = rhs.c; r = rhs.r; }
    return *this;
}

std::string Sphere::to_string() const {
    std::ostringstream o;
    o << "Sphere(c=" << c.to_string() << ", r=" << r << ")";
    return o.str();
}

bool Sphere::hit(const Ray &ray, float &t, ShadeInfo &s) const {
    // Vector from ray origin to sphere center
    Vector3D oc = ray.o - c;  // Point3D - Point3D = Vector3D

    float a = (float)(ray.d * ray.d);
    float b = (float)(2.0 * (oc * ray.d));
    float c_coef = (float)(oc * oc) - r * r;

    float discriminant = b * b - 4.0f * a * c_coef;

    if (discriminant < 0) return false;

    float sqrt_disc = std::sqrt(discriminant);
    float t0 = (-b - sqrt_disc) / (2.0f * a);
    float t1 = (-b + sqrt_disc) / (2.0f * a);

    float t_hit = -1.0f;
    if (t0 > kEpsilon) t_hit = t0;
    else if (t1 > kEpsilon) t_hit = t1;
    else return false;

    t = t_hit;
    s.hit = true;
    s.t = t_hit;
    s.hit_point = ray.o + t_hit * ray.d;
    s.normal = (s.hit_point - c);
    s.normal.normalize();
    s.ray = ray;
    s.material_ptr = material_ptr;
    return true;
}

BBox Sphere::getBBox() const {
    return BBox(Point3D(c.x - r, c.y - r, c.z - r),
                Point3D(c.x + r, c.y + r, c.z + r));
}

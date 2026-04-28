#include "Plane.hpp"
#include "../utilities/BBox.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Constants.hpp"
#include <sstream>

Plane::Plane() : a(0, 0, 0), n(0, 1, 0) {}

Plane::Plane(const Point3D &pt, const Vector3D &normal) : a(pt), n(normal) {
    n.normalize();
}

Plane::Plane(const Plane &obj) : Geometry(obj), a(obj.a), n(obj.n) {}

Plane &Plane::operator=(const Plane &rhs) {
    if (this != &rhs) { Geometry::operator=(rhs); a = rhs.a; n = rhs.n; }
    return *this;
}

std::string Plane::to_string() const {
    std::ostringstream o;
    o << "Plane(a=" << a.to_string() << ", n=" << n.to_string() << ")";
    return o.str();
}

bool Plane::hit(const Ray &ray, float &t, ShadeInfo &s) const {
    // t = (a - o) . n / (d . n)
    float denom = (float)(ray.d * n);
    if (std::abs(denom) < kEpsilon) return false;

    Vector3D diff = a - ray.o;  // Point3D - Point3D = Vector3D
    float t_hit = (float)(diff * n) / denom;

    if (t_hit < kEpsilon) return false;

    t = t_hit;
    s.hit = true;
    s.t = t_hit;
    s.hit_point = ray.o + t_hit * ray.d;
    s.normal = n;
    s.ray = ray;
    s.material_ptr = material_ptr;
    return true;
}

BBox Plane::getBBox() const {
    // Planes are infinite; return a huge bounding box
    const float huge = 1e6f;
    return BBox(Point3D(-huge, -huge, -huge), Point3D(huge, huge, huge));
}

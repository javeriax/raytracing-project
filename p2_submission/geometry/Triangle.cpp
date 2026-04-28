#include "Triangle.hpp"
#include "../utilities/BBox.hpp"
#include "../utilities/Ray.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Constants.hpp"
#include <sstream>

Triangle::Triangle() : v0(0,0,0), v1(0,0,0), v2(0,0,0) {}

Triangle::Triangle(const Point3D &a, const Point3D &b, const Point3D &c)
    : v0(a), v1(b), v2(c) {}

Triangle::Triangle(const Triangle &obj)
    : Geometry(obj), v0(obj.v0), v1(obj.v1), v2(obj.v2) {}

Triangle &Triangle::operator=(const Triangle &rhs) {
    if (this != &rhs) {
        Geometry::operator=(rhs);
        v0 = rhs.v0; v1 = rhs.v1; v2 = rhs.v2;
    }
    return *this;
}

std::string Triangle::to_string() const {
    std::ostringstream o;
    o << "Triangle(" << v0.to_string() << ", " << v1.to_string() << ", " << v2.to_string() << ")";
    return o.str();
}

bool Triangle::hit(const Ray &ray, float &t, ShadeInfo &s) const {
    // Möller–Trumbore algorithm
    Vector3D edge1 = v1 - v0;
    Vector3D edge2 = v2 - v0;

    Vector3D h = ray.d ^ edge2;
    double a = edge1 * h;

    if (std::abs(a) < kEpsilon) return false;

    double f = 1.0 / a;
    Vector3D sv = ray.o - v0;
    double u = f * (sv * h);
    if (u < 0.0 || u > 1.0) return false;

    Vector3D q = sv ^ edge1;
    double v = f * (ray.d * q);
    if (v < 0.0 || u + v > 1.0) return false;

    double t_hit = f * (edge2 * q);
    if (t_hit < kEpsilon) return false;

    t = (float)t_hit;
    s.hit = true;
    s.t = (float)t_hit;
    s.hit_point = ray.o + t * ray.d;
    s.normal = edge1 ^ edge2;
    s.normal.normalize();
    s.ray = ray;
    s.material_ptr = material_ptr;
    return true;
}

BBox Triangle::getBBox() const {
    Point3D mn = min(min(v0, v1), v2);
    Point3D mx = max(max(v0, v1), v2);
    return BBox(mn, mx);
}

#include "BBox.hpp"
#include "Ray.hpp"
#include "../geometry/Geometry.hpp"
#include <cmath>
#include <sstream>
#include <algorithm>

BBox::BBox(const Point3D &mn, const Point3D &mx) : pmin(mn), pmax(mx) {}

std::string BBox::to_string() const {
    std::ostringstream o;
    o << "BBox(" << pmin.to_string() << ", " << pmax.to_string() << ")";
    return o.str();
}

bool BBox::hit(const Ray &ray, float &t_enter, float &t_exit) const {
    float ox = (float)ray.o.x, oy = (float)ray.o.y, oz = (float)ray.o.z;
    float dx = (float)ray.d.x, dy = (float)ray.d.y, dz = (float)ray.d.z;

    float tx_min, tx_max, ty_min, ty_max, tz_min, tz_max;

    float a = 1.0f / dx;
    if (a >= 0) { tx_min = (pmin.x - ox) * a; tx_max = (pmax.x - ox) * a; }
    else        { tx_min = (pmax.x - ox) * a; tx_max = (pmin.x - ox) * a; }

    float b = 1.0f / dy;
    if (b >= 0) { ty_min = (pmin.y - oy) * b; ty_max = (pmax.y - oy) * b; }
    else        { ty_min = (pmax.y - oy) * b; ty_max = (pmin.y - oy) * b; }

    float c = 1.0f / dz;
    if (c >= 0) { tz_min = (pmin.z - oz) * c; tz_max = (pmax.z - oz) * c; }
    else        { tz_min = (pmax.z - oz) * c; tz_max = (pmin.z - oz) * c; }

    t_enter = std::max({tx_min, ty_min, tz_min});
    t_exit  = std::min({tx_max, ty_max, tz_max});

    return t_enter < t_exit;
}

void BBox::extend(Geometry *g) {
    BBox b = g->getBBox();
    pmin = min(pmin, b.pmin);
    pmax = max(pmax, b.pmax);
}

void BBox::extend(const BBox &b) {
    pmin = min(pmin, b.pmin);
    pmax = max(pmax, b.pmax);
}

bool BBox::contains(const Point3D &p) {
    return p.x >= pmin.x && p.x <= pmax.x &&
           p.y >= pmin.y && p.y <= pmax.y &&
           p.z >= pmin.z && p.z <= pmax.z;
}

bool BBox::overlaps(Geometry *g) {
    return overlaps(g->getBBox());
}

bool BBox::overlaps(const BBox &b) {
    return pmin.x <= b.pmax.x && pmax.x >= b.pmin.x &&
           pmin.y <= b.pmax.y && pmax.y >= b.pmin.y &&
           pmin.z <= b.pmax.z && pmax.z >= b.pmin.z;
}

#include "BVH.hpp"
#include "../geometry/Geometry.hpp"
#include "../utilities/BBox.hpp"
#include "../utilities/ShadeInfo.hpp"
#include "../utilities/Constants.hpp"
#include "../world/World.hpp"
#include <algorithm>
#include <cassert>

BVH::BVH(const World *w) : world_ptr(w) {}

// entry point - builds bvh from all geometry
void BVH::build(const std::vector<Geometry *> &objects)
{
    if (objects.empty())
        return;

    // make a mutable copy to sort during build
    std::vector<Geometry *> objs(objects);
    root = build_recursive(objs, 0, (int)objs.size(), 0);
}

// recursively builds the bvh tree using surface area heuristic splitting
// splits objects along the longest axis of their combined bounding box
std::unique_ptr<BVHNode> BVH::build_recursive(
    std::vector<Geometry *> &objects,
    int start, int end, int depth)
{
    auto node = std::make_unique<BVHNode>();
    int count = end - start;

    // compute bounding box for all objects in this range
    BBox combined = objects[start]->getBBox();
    for (int i = start + 1; i < end; i++)
    {
        combined.extend(objects[i]);
    }
    node->bbox = combined;

    // leaf condition: 4 or fewer objects, or max depth reached
    if (count <= 4 || depth > 32)
    {
        node->is_leaf = true;
        for (int i = start; i < end; i++)
        {
            node->objects.push_back(objects[i]);
        }
        return node;
    }

    // find the longest axis to split along (x=0, y=1, z=2)
    float dx = combined.pmax.x - combined.pmin.x;
    float dy = combined.pmax.y - combined.pmin.y;
    float dz = combined.pmax.z - combined.pmin.z;
    int axis = (dx > dy && dx > dz) ? 0 : (dy > dz ? 1 : 2);

    // sort objects along chosen axis by their centroid position
    std::sort(objects.begin() + start, objects.begin() + end,
              [axis](Geometry *a, Geometry *b)
              {
                  BBox ba = a->getBBox();
                  BBox bb = b->getBBox();
                  float ca, cb;
                  if (axis == 0)
                  {
                      ca = (ba.pmin.x + ba.pmax.x) * 0.5f;
                      cb = (bb.pmin.x + bb.pmax.x) * 0.5f;
                  }
                  else if (axis == 1)
                  {
                      ca = (ba.pmin.y + ba.pmax.y) * 0.5f;
                      cb = (bb.pmin.y + bb.pmax.y) * 0.5f;
                  }
                  else
                  {
                      ca = (ba.pmin.z + ba.pmax.z) * 0.5f;
                      cb = (bb.pmin.z + bb.pmax.z) * 0.5f;
                  }
                  return ca < cb;
              });

    // split in the middle
    int mid = start + count / 2;
    node->is_leaf = false;
    node->left = build_recursive(objects, start, mid, depth + 1);
    node->right = build_recursive(objects, mid, end, depth + 1);

    return node;
}

// public hit - starts recursive search from root
ShadeInfo BVH::hit(const Ray &ray) const
{
    ShadeInfo best(*world_ptr);
    float t_min = kHugeValue;

    if (root)
    {
        hit_recursive(root.get(), ray, t_min, best);
    }
    return best;
}

// recursively traverses bvh to find closest hit
bool BVH::hit_recursive(const BVHNode *node, const Ray &ray,
                        float &t_min, ShadeInfo &best) const
{
    // first check if ray hits this node's bounding box at all
    float t_enter, t_exit;
    if (!node->bbox.hit(ray, t_enter, t_exit))
    {
        return false; // ray misses this whole subtree - skip it
    }

    // if bounding box hit is already further than current best, skip
    if (t_enter > t_min)
        return false;

    bool hit_anything = false;

    if (node->is_leaf)
    {
        // check all objects in this leaf
        for (auto g : node->objects)
        {
            float t;
            ShadeInfo si(*world_ptr);
            if (g->hit(ray, t, si) && t < t_min)
            {
                t_min = t;
                best = si;
                hit_anything = true;
            }
        }
    }
    else
    {
        // check both children and recurse
        bool hit_left = hit_recursive(node->left.get(), ray, t_min, best);
        bool hit_right = hit_recursive(node->right.get(), ray, t_min, best);
        hit_anything = hit_left || hit_right;
    }

    return hit_anything;
}

// public shadow check - just needs any hit before max_t
bool BVH::shadow_hit(const Ray &ray, float max_t) const
{
    if (root)
    {
        return shadow_recursive(root.get(), ray, max_t);
    }
    return false;
}

// shadow version - stops as soon as any blocker is found
bool BVH::shadow_recursive(const BVHNode *node, const Ray &ray,
                           float max_t) const
{
    float t_enter, t_exit;
    if (!node->bbox.hit(ray, t_enter, t_exit))
    {
        return false;
    }

    // if bvh bbox entry is past max_t, light is closer than this subtree
    if (t_enter > max_t)
        return false;

    if (node->is_leaf)
    {
        float t;
        for (auto g : node->objects)
        {
            ShadeInfo si(*world_ptr);
            if (g->hit(ray, t, si) && t < max_t && t > kEpsilon)
            {
                return true; // found a blocker - in shadow
            }
        }
        return false;
    }

    // check children - return true as soon as either finds a blocker
    return shadow_recursive(node->left.get(), ray, max_t) || shadow_recursive(node->right.get(), ray, max_t);
}

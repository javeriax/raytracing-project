#pragma once

#include "Acceleration.hpp"
#include "../utilities/BBox.hpp"
#include <vector>
#include <memory>

class Geometry;

// bvh node - each node is either a leaf (holds geometry)
// or an interior node (holds two children and a bounding box)
struct BVHNode {
    BBox bbox;                          // bounding box wrapping this node
    std::unique_ptr<BVHNode> left;      // left child
    std::unique_ptr<BVHNode> right;     // right child
    std::vector<Geometry*> objects;     // only filled for leaf nodes
    bool is_leaf;                       // true if this node holds objects

    BVHNode() : is_leaf(false) {}
};

// bvh = bounding volume hierarchy
// organizes geometry into a tree of bounding boxes
// a ray only needs to check objects whose bounding boxes it hits
// this turns O(n) intersection tests into O(log n)
class BVH : public Acceleration {
private:
    std::unique_ptr<BVHNode> root;  // root of the bvh tree
    const World* world_ptr;         // needed for shadeinfo construction

    // recursively builds the bvh tree
    std::unique_ptr<BVHNode> build_recursive(
        std::vector<Geometry*>& objects,
        int start, int end, int depth);

    // recursively finds closest hit in the tree
    bool hit_recursive(const BVHNode* node, const Ray& ray,
                       float& t_min, ShadeInfo& best) const;

    // recursively checks for any shadow hit in the tree
    bool shadow_recursive(const BVHNode* node, const Ray& ray,
                          float max_t) const;

public:
    BVH(const World* w);

    virtual void build(const std::vector<Geometry*>& objects) override;
    virtual ShadeInfo hit(const Ray& ray) const override;
    virtual bool shadow_hit(const Ray& ray, float max_t) const override;
};

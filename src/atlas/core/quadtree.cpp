#include "quadtree.hpp"

using namespace atlas::core;

Quadtree::Quadtree(const Region& region, uint32_t subdivX, uint32_t subdivY) :
    _root(std::make_unique<QuadtreeNode>(region, QuadtreeNode::Coord{ 0, 0, 0 }, nullptr))
{
    _root->subdivide(subdivX, subdivY);
}


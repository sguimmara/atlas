#include "quadtree.hpp"

using namespace atlas::core;
using namespace atlas::core::spatialindex;

Quadtree::Quadtree(const Region& region, uint32_t subdivX, uint32_t subdivY) :
    _subdivX(subdivX),
    _subdivY(subdivY),
    _root(std::make_unique<QuadtreeNode>(region, QuadtreeNode::Key(0, 0, 0), nullptr))
{
    //_root->subdivide(subdivX, subdivY);
}

void Quadtree::evaluate(std::function<bool(const QuadtreeNode&)> predicate)
{
    clear();

    _root->evaluate(predicate);
}

void Quadtree::clear()
{
    _root->clear();
}

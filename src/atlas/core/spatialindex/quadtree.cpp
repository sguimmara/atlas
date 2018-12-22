#include "quadtree.hpp"

using namespace atlas::core;
using namespace atlas::core::spatialindex;

Quadtree::Quadtree(const Region& region, uint32_t subdivX, uint32_t subdivY) :
    _regular(subdivX == 1 || subdivY == 1),
    _root(QuadtreeNode(region, QuadtreeNode::Key(0, 0, 0)))
{
    if (!_regular)
    {
        _root.subdivide(subdivX, subdivY);
    }
}

QuadtreeNode::iterator Quadtree::evaluate(std::function<bool(const QuadtreeNode&)> predicate)
{
    if (!_regular)
    {
        _root.evaluateChildren(predicate);
    }
    else
    {
        _root.evaluate(predicate);
    }

    return QuadtreeNode::iterator(&_root);
}

void Quadtree::clear()
{
    _root.clear();
}

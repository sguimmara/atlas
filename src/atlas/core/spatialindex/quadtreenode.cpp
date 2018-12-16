#include "quadtreenode.hpp"

using namespace atlas::core;
using namespace atlas::core::spatialindex;

QuadtreeNode::QuadtreeNode(Region region, Key coord) :
    _key(coord),
    _region(region)
{}

void QuadtreeNode::subdivide(uint32_t x, uint32_t y)
{
    assert(x > 0);
    assert(y > 0);

    _children.clear();

    uint32_t col = _key.col() * 2;
    uint32_t row = _key.row() * 2;
    uint32_t depth = _key.depth() + 1;

    auto subregions = _region.subdivide(x, y);

    for (uint32_t i = 0; i < x; i++)
    {
        for (uint32_t j = 0; j < y; j++)
        {
            auto const subregion = subregions[i + j * x];
            auto const coord = Key(col + i, row + j, depth);
            _children.emplace_back(subregion, coord);
        }
    }

    assert(_children.size() == x * y);
}

void QuadtreeNode::split()
{
    if (_children.empty())
    {
        uint32_t col = _key.col() * 2;
        uint32_t row = _key.row() * 2;
        uint32_t depth = _key.depth() + 1;

        _children.emplace_back(_region.northWest(), Key(col, row, depth));
        _children.emplace_back(_region.northEast(), Key(col + 1, row, depth));
        _children.emplace_back(_region.southWest(), Key(col, row + 1, depth));
        _children.emplace_back(_region.southEast(), Key(col + 1, row + 1, depth));
    }
    else
    {
        assert(_children.size() > 1);

        for (auto& child : _children)
        {
            child.split();
        }
    }
}

void QuadtreeNode::evaluate(std::function<bool(const QuadtreeNode&)> predicate)
{
    clear();
    if (predicate(*this))
    {
        split();
        evaluateChildren(predicate);
    }
}

void QuadtreeNode::evaluateChildren(std::function<bool(const QuadtreeNode&)> predicate)
{
    for (auto& child : _children)
    {
        child.evaluate(predicate);
    }
}

QuadtreeNode::iterator::iterator(QuadtreeNode * current) :
    _current(current)
{
    _stack.emplace(current, 0);
}

void QuadtreeNode::iterator::operator++()
{
    if (_current->isleaf())
    {
        if (_stack.empty())
        {
            // end of iterator
            _current = nullptr;
            return;
        }
        else
        {
            auto top = _stack.top();
            _stack.pop();
            top.second++;
            if (top.second < top.first->_children.size())
            {
                _stack.push(top);
                _current = &top.first->_children[top.second];
            }
        }
    }
    else
    {
        _stack.push({ _current, 0 });
        _current = &_current->_children[0];
    }
}

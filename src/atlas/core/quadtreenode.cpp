#include "quadtreenode.hpp"

using namespace atlas::core;

QuadtreeNode::QuadtreeNode(Region region, Key coord, QuadtreeNode* parent) :
    _key(coord),
    _region(region),
    _parent(parent),
    _rightSibling(nullptr)
{}

void QuadtreeNode::subdivide(uint32_t x, uint32_t y)
{
    assert(x > 0);
    assert(y > 0);

    _children.clear();

    uint32_t col = _key.col * 2;
    uint32_t row = _key.row * 2;
    uint32_t depth = _key.depth + 1;

    auto subregions = _region.subdivide(x, y);

    for (uint32_t i = 0; i < x; i++)
    {
        for (uint32_t j = 0; j < y; j++)
        {
            auto const subregion = subregions[i + j * x];
            auto const coord = Key{ col + i, row + j, depth };
            _children.emplace_back(std::make_unique<QuadtreeNode>(subregion, coord, this));
        }
    }

    for (size_t i = 0; i < _children.size() - 1; i++)
    {
        _children[i]->_rightSibling = _children[i + 1].get();
    }
    _children[_children.size() - 1]->_rightSibling = nullptr;

    assert(_children.size() == x * y);
}

void QuadtreeNode::split()
{
    if (_children.empty())
    {
        uint32_t col = _key.col * 2;
        uint32_t row = _key.row * 2;
        uint32_t depth = _key.depth + 1;

        QuadtreeNode* nw = new QuadtreeNode(_region.northWest(), { col, row, depth }, this);
        QuadtreeNode* ne = new QuadtreeNode(_region.northEast(), { col + 1, row, depth }, this);
        QuadtreeNode* sw = new QuadtreeNode(_region.southWest(), { col, row, depth }, this);
        QuadtreeNode* se = new QuadtreeNode(_region.northEast(), { col + 1, row + 1, depth }, this);

        nw->_rightSibling = ne;
        ne->_rightSibling = sw;
        sw->_rightSibling = se;
        se->_rightSibling = nullptr;

        _children.push_back(std::unique_ptr<QuadtreeNode>(nw));
        _children.push_back(std::unique_ptr<QuadtreeNode>(ne));
        _children.push_back(std::unique_ptr<QuadtreeNode>(sw));
        _children.push_back(std::unique_ptr<QuadtreeNode>(se));
    }
    else
    {
        assert(_children.size() > 1);

        for (auto& child : _children)
        {
            child->split();
        }
    }
}

QuadtreeNode::iterator::iterator(QuadtreeNode * current) : _current(current)
{}

void QuadtreeNode::iterator::operator++()
{
    if (!_current)
    {
        return;
    }

    if (_current->isleaf())
    {
        if (_current->_rightSibling)
        {
            _current = _current->_rightSibling;
        }
        else
        {
            _current = _current->_parent->_rightSibling;
        }
    }
    else
    {
        _current = _current->_children[0].get();
    }
}

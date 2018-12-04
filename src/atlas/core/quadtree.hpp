#ifndef ATLAS_CORE_QUADTREE_HPP
#define ATLAS_CORE_QUADTREE_HPP

#include "common.hpp"
#include "region.hpp"
#include "quadtreenode.hpp"

namespace atlas::core
{
    class Quadtree
    {
        typedef QuadtreeNode::iterator iterator;

    public:
        Quadtree(const Region&, uint32_t subdivX, uint32_t subdivY);

        inline iterator begin() noexcept { return iterator(_root.get()); }
        inline iterator end() noexcept { return iterator(nullptr); }

    private:
        std::unique_ptr<QuadtreeNode> _root;
    };
}

#endif // ATLAS_CORE_QUADTREE_HPP
#ifndef ATLAS_CORE_SPATIALINDEX_QUADTREE_HPP
#define ATLAS_CORE_SPATIALINDEX_QUADTREE_HPP

#include "../common.hpp"
#include "../region.hpp"
#include "quadtreenode.hpp"

namespace atlas::core::spatialindex
{
    class Quadtree
    {
        typedef QuadtreeNode::iterator iterator;

    public:
        Quadtree(const Region&, uint32_t subdivX, uint32_t subdivY);

        // begin a traversal of the tree, applying the evaluation function on each
        // node. The evaluation function returns true if the node must be subdivided,
        // false otherwise. Before each traversal, the tree is reset, all nodes
        // are removed, except for the root node. Once completed, leaf nodes can
        // be retrieved.
        void evaluate(std::function<bool(const QuadtreeNode&)>);
        
        // removes all nodes from the tree, except for the root nodes.
        void clear();

        inline iterator begin() noexcept { return iterator(_root.get()); }
        inline iterator end() noexcept { return iterator(nullptr); }

    private:
        uint32_t _subdivX;
        uint32_t _subdivY;
        std::unique_ptr<QuadtreeNode> _root;

        void evaluate(QuadtreeNode&, std::function<bool(const QuadtreeNode&)>);
    };
}

#endif // ATLAS_CORE_SPATIALINDEX_QUADTREE_HPP
#ifndef ATLAS_CORE_SPATIALINDEX_QUADTREE_HPP
#define ATLAS_CORE_SPATIALINDEX_QUADTREE_HPP

#include "../common.hpp"
#include "../region.hpp"
#include "quadtreenode.hpp"
#include "evaluator.hpp"

namespace atlas::core::spatialindex
{
    class Quadtree
    {
        typedef QuadtreeNode::iterator iterator;

    public:
        Quadtree(const Region&, uint32_t subdivX, uint32_t subdivY);

        // evaluates the tree using the provided predicate. The predicate must
        // return true whenever a given node must be splitted into four children
        // otherwise false. Before the traversal, the tree is cleared (i.e only
        // the root node is kept). If this tree is irregular, the predicate will
        // not be applied to the root node, but instead on its children. Returns
        // an iterator for this tree.
        iterator evaluate(const Evaluator<QuadtreeNode>& evaluator);

        // returns true if this tree is a regular quadtree, i.e its root is
        // split into four children. Some tiling schemes require an irregular
        // quadtree, for example the GoogleCRS84Quad. Note that this only applies
        // to the root node. Every other node must have four children.
        inline bool regular() const noexcept { return _regular; }

        // removes all nodes from the tree, except for the root nodes.
        void clear();

        inline iterator begin() noexcept { return iterator(&_root); }
        inline iterator end() noexcept { return iterator(nullptr); }

    private:
        bool _regular;
        QuadtreeNode _root;
    };
}

#endif // ATLAS_CORE_SPATIALINDEX_QUADTREE_HPP
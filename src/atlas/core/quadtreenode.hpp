#ifndef ATLAS_CORE_QUADTREENODE_HPP
#define ATLAS_CORE_QUADTREENODE_HPP

#include "common.hpp"
#include "region.hpp"

namespace atlas::core
{
    class QuadtreeNode
    {
    public:
        // the key of a quadtree node in the tree
        struct Key
        {
            uint32_t col;
            uint32_t row;
            uint32_t depth;

            bool operator==(const Key& rhs) const
            {
                return rhs.col == col && rhs.row == row && rhs.depth == depth;
            }

            bool operator!=(const Key& rhs) const
            {
                return !(*this == rhs);
            }
        };

        class iterator
        {
        public:
            iterator(QuadtreeNode* root);
            void operator++();
            inline QuadtreeNode& operator*() noexcept { return *_current; }
            inline bool operator==(const iterator& rhs) const noexcept { return _current == rhs._current; }
            inline bool operator!=(const iterator& rhs) const noexcept
            {
                return _current != rhs._current;
            }

        private:
            QuadtreeNode* _current;
        };

    public:
        QuadtreeNode(Region region, Key coord, QuadtreeNode* parent);

        inline Region region() const noexcept { return _region; }
        inline QuadtreeNode* parent() const noexcept { return _parent; }
        inline Key key() const noexcept { return _key; }
        bool isleaf() const noexcept { return _children.empty(); }

        void subdivide(uint32_t x, uint32_t y);
        void split();

    private:
        Key _key;
        Region _region;
        QuadtreeNode* _parent;
        QuadtreeNode* _rightSibling;
        std::vector<std::unique_ptr<QuadtreeNode>> _children;
    };
}

namespace std
{
    typedef atlas::core::QuadtreeNode::Key item;
    template <>
    struct hash<item>
    {
        inline size_t operator()(const item& coord) const
        {
            return (coord.col * 23) ^ (coord.depth * 17) ^ (coord.row * 7);
        }
    };
}

#endif // ATLAS_CORE_QUADTREENODE_HPP
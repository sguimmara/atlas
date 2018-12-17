#ifndef ATLAS_CORE_SPATIALINDEX_QUADTREENODE_HPP
#define ATLAS_CORE_SPATIALINDEX_QUADTREENODE_HPP

#include "../common.hpp"
#include "../region.hpp"
#include <stack>
#include <iterator>

namespace atlas::core::spatialindex
{
    class QuadtreeNode
    {
    public:
        // the key of a quadtree node in the tree
        class Key
        {
        public:
            inline uint32_t col() const noexcept { return _col; }
            inline uint32_t row() const noexcept { return _row; }
            inline uint32_t depth() const noexcept { return _depth; }

            Key(uint32_t col, uint32_t row, uint32_t depth) :
                _col(col), _row(row), _depth(depth)
            {}

            bool operator==(const Key& rhs) const noexcept
            {
                return rhs._col == _col && rhs._row == _row && rhs._depth == _depth;
            }

            bool operator!=(const Key& rhs) const noexcept
            {
                return !(*this == rhs);
            }

        private:
            uint32_t _col;
            uint32_t _row;
            uint32_t _depth;
        };

        class iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = QuadtreeNode;
            using difference_type = std::ptrdiff_t;
            using pointer = QuadtreeNode*;
            using reference = QuadtreeNode&;

            iterator(QuadtreeNode* root);
            void operator++();
            inline QuadtreeNode& operator*() noexcept { return *_current; }
            inline bool operator==(const iterator& rhs) const noexcept { return _current == rhs._current; }
            inline bool operator!=(const iterator& rhs) const noexcept { return _current != rhs._current; }

        private:
            QuadtreeNode* _current;
            std::stack<std::pair<QuadtreeNode*, size_t>> _stack;
        };

    public:
        QuadtreeNode(Region region, Key coord);

        inline Region region() const noexcept { return _region; }
        inline Key key() const noexcept { return _key; }
        bool isleaf() const noexcept { return _children.empty(); }
        void clear() noexcept { _children.clear(); }
        inline bool operator==(const QuadtreeNode& rhs) { return rhs._key == _key; }
        inline bool operator!=(const QuadtreeNode& rhs) { return rhs._key != _key; }

        iterator begin() noexcept { return iterator(this); }
        iterator end() noexcept { return iterator(nullptr); }
        void subdivide(uint32_t x, uint32_t y);
        void split();
        void evaluate(std::function<bool(const QuadtreeNode&)> predicate);
        void evaluateChildren(std::function<bool(const QuadtreeNode&)> predicate);

    private:
        Key _key;
        Region _region;
        std::vector<QuadtreeNode> _children;
    };
}

namespace std
{
    typedef atlas::core::spatialindex::QuadtreeNode::Key item;
    template <>
    struct hash<item>
    {
        inline size_t operator()(const item& coord) const
        {
            return (coord.col() * 23) ^ (coord.depth() * 17) ^ (coord.row() * 7);
        }
    };
}

#endif // ATLAS_CORE_SPATIALINDEX_QUADTREENODE_HPP
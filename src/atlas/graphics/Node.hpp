#ifndef ATLAS_GRAPHICS_NODE_HPP
#define ATLAS_GRAPHICS_NODE_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {
        class Camera;
        class Node;

        struct DrawContext
        {
            vk::CommandBuffer cmdBuffer;
            Transform viewMatrix;
            Transform projectionMatrix;
        };

        struct UpdateContext
        {
            Node* camera;
        };

        enum class NodeFlags
        {
            None = 0,
            Traversable = 1,
            Drawable = 2,
        };

        /**
        * @brief      Provides preorder traversal functionality for nodes.
        */
        class NodeIterator
        {
        public:
            NodeIterator(Node* current);

            /**
            * @brief      Moves to the next Node in the preorder traversal.
            *
            * @return     The next Node
            */
            Node* operator++();

            /**
            * @brief      Returns the current Node before moving to the
            *             next Node in the preorder traversal.
            *
            * @return     the Node before the current Node.
            */
            Node* operator++(int);

            /**
            * @brief      Returns the current Node.
            *
            * @return     the current Node.
            */
            inline Node* operator*() const
            {
                return _current;
            }

            bool operator==(const NodeIterator& rhs)
            {
                return _current == rhs._current;
            }

            bool operator!=(const NodeIterator& rhs)
            {
                return _current != rhs._current;
            }

        private:
            Node * _current;
        };

        /**
         * @brief      Represents an element of a scene graph.
         */
        class Node
        {
        public:
            /**
             * @brief      Constructs a Node.
             */
            Node();
            virtual ~Node() {}

            /**
             * @brief      Returns the number of elements in this Node, recursively.
             *
             * @return     The number of elements in this Node.
             */
            size_t size() const noexcept;

            /**
             * @brief      Adds a child to this Node. The new child becomes
             *             the rightmost child.
             *
             * @param      Child  the value to add as a child.
             */
            void add_child(Node* child);

            /**
             * @brief      Returns the number of children in this Node.
             *
             * @return     The number of children.
             */
            inline size_t child_count() const noexcept
            {
                return _children.size();
            }

            /**
             * @brief      returns the parent of this Node, if any,
             *             otherwise returns the nullptr.
             *
             * @return     the parent, or the nullptr.
             */
            inline Node* parent() const noexcept
            {
                return _parent;
            }

            /**
             * @brief      returns the right sibling of this Node, if any,
             *             otherwise returns the nullptr.
             *
             * @return     the right sibling, or the nullptr.
             */
            inline Node* right() const noexcept
            {
                return _rightSibling;
            }

            /**
             * @brief      removes all children from this Node.
             */
            inline void clear_children() noexcept
            {
                _children.clear();
            }

            /**
             * @brief      Returns the child at index pos.
             *
             * @param[in]  pos   The child index.
             *
             *
             * @throw std::invalid_argument if pos is out of bounds.
             * @return     The child.
             */
            Node* get_child(size_t pos) const;

            /**
             * @brief      Removes the child at index pos.
             *
             * @param[in]  pos   The child index.
             *
             * @throw std::invalid_argument if pos is out of bounds.
             */
            void remove_child(size_t pos);

            virtual void Update(UpdateContext ctx) {}

            /**************************************************************/
            /*                          operators                         */
            /**************************************************************/
            // friend std::ostream& operator<< (
            //     std::ostream& os,
            //     const Node& g)
            // {

            //     return os;
            //     //return os << "(" << *g.data << ")";
            // }

            //bool operator ==(const Node& rhs)
            //{
            //    return mValue == rhs.mValue
            //        && _parent == rhs._parent
            //        && _rightSibling == rhs._rightSibling
            //        && _children == rhs._children;
            //}

            //bool operator !=(const Node& rhs)
            //{
            //    return mValue != rhs
            //        || _parent != rhs._parent
            //        || _rightSibling != rhs._rightSibling
            //        || _children != rhs._children;
            //}

            /**
             * @brief      Returns an iterator starting at this Node.
             *
             * @return     the iterator for this Node.
             */
            NodeIterator begin() noexcept;

            /**
             * @brief      Returns an iterator at the end of the preorder
             *             traversal, i.e at the right of the rightmost branch.
             *
             * @return     the iterator for the end.
             */
            NodeIterator end() noexcept;

            inline const int flags() const noexcept { return _flags; }

            /**************************************************************/
            /*                          transform                         */
            /**************************************************************/
            inline void setTransform(Transform t) noexcept { _transform = t; }
            inline void setLocalTransform(Transform t) noexcept { _localTransform = t; }
            inline Transform localTransform() const noexcept { return _localTransform; }
            inline Transform transform() const noexcept { return _transform; }

        protected:
            glm::mat4 _transform;
            glm::mat4 _localTransform;

            Node* _parent;
            Node* _rightSibling;
            std::vector<Node*> _children;
            int _flags;

            void throw_if_out_of_range(size_t child_pos) const;
        };

    }
}

#endif // !ATLAS_GRAPHICS_SCENEGRAPH_HPP

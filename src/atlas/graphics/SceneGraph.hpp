#ifndef ATLAS_GRAPHICS_SCENEGRAPH_HPP
#define ATLAS_GRAPHICS_SCENEGRAPH_HPP

#include "AtlasGraphics.hpp"

namespace atlas
{
    namespace graphics
    {
        template<typename T>
        class node_iterator;

        /**
         * @brief      Represents an element of a scene graph.
         *
         * @tparam     T     the Node's content type.
         */
        template<typename T>
        class Node
        {
            typedef node_iterator<T> iterator;
            typedef const node_iterator<T> const_iterator;

        public:
            /**
             * @brief      Constructs a Node containing the specified value.
             *
             * @param      value  The value stored in this Node.
             */
            Node(T* value) :
                mValue(value),
                mParent(nullptr),
                mRightSibling(nullptr)
            {
                if (nullptr == value)
                {
                    throw std::invalid_argument("value cannot be null");
                }
            }

            /**
             * @brief      Returns the number of elements in this Node, recursively.
             *
             * @return     The number of elements in this Node.
             */
            size_t size() const noexcept
            {
                size_t size = 1;
                for (auto const& child : mChildren)
                {
                    size += child->size();
                }
                return size;
            }

            /**
             * @brief      Returns the underlying value.
             *
             * @return     The underlying value.
             */
            inline T& value() const noexcept
            {
                return *mValue;
            }

            /**
             * @brief      Adds a child to this Node. The new child becomes
             *             the rightmost child.
             *
             * @param      Child  the value to add as a child.
             */
            void add_child(T* child)
            {
                if (child == nullptr)
                {
                    throw std::invalid_argument("child is null.");
                }

                auto child_ptr = std::make_unique<Node<T>>(child);
                child_ptr->mParent = this;
                if (!mChildren.empty())
                {
                    mChildren.back()->mRightSibling = child_ptr.get();
                }
                mChildren.push_back(std::move(child_ptr));
            }

            /**
             * @brief      Returns the number of children in this Node.
             *
             * @return     The number of children.
             */
            inline size_t child_count() const noexcept
            {
                return mChildren.size();
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
            Node<T>& get_child(size_t pos) const
            {
                throw_if_out_of_range(pos);

                return *mChildren[pos];
            }

            /**
             * @brief      Removes the child at index pos.
             *
             * @param[in]  pos   The child index.
             *
             * @throw std::invalid_argument if pos is out of bounds.
             */
            void remove_child(size_t pos)
            {
                throw_if_out_of_range(pos);

                mChildren.erase(mChildren.begin() + pos);
            }

            /**
             * @brief      returns the parent of this Node, if any,
             *             otherwise returns the nullptr.
             *
             * @return     the parent, or the nullptr.
             */
            inline Node<T>* parent() const noexcept
            {
                return mParent;
            }

            /**
             * @brief      returns the right sibling of this Node, if any,
             *             otherwise returns the nullptr.
             *
             * @return     the right sibling, or the nullptr.
             */
            inline Node<T>* right() const noexcept
            {
                return mRightSibling;
            }

            /**
             * @brief      removes all children from this Node.
             */
            inline void clear_children() noexcept
            {
                mChildren.clear();
            }

            /**************************************************************/
            /*                          operators                         */
            /**************************************************************/
            friend std::ostream& operator<< (
                std::ostream& os,
                const Node<T>& g)
            {
                return os << "(" << *g.data << ")";
            }

            bool operator ==(const Node<T>& rhs)
            {
                return mValue == rhs.mValue
                    && mParent == rhs.mParent
                    && mRightSibling == rhs.mRightSibling
                    && mChildren == rhs.mChildren;
            }

            bool operator !=(const Node<T>& rhs)
            {
                return mValue != rhs.mValue
                    || mParent != rhs.mParent
                    || mRightSibling != rhs.mRightSibling
                    || mChildren != rhs.mChildren;
            }

            T& operator*()
            {
                return *mValue;
            }

            /**
             * @brief      Returns an iterator starting at this Node.
             *
             * @return     the iterator for this Node.
             */
            inline iterator begin() noexcept
            {
                return iterator(this);
            }

            /**
             * @brief      Returns a constant iterator starting at this Node.
             *
             * @return     the iterator for this Node.
             */
            inline const_iterator begin() const noexcept
            {
                return iterator(this);
            }

            /**
             * @brief      Returns an iterator at the end of the preorder
             *             traversal, i.e at the right of the rightmost branch.
             *
             * @return     the iterator for the end.
             */
            inline iterator end() noexcept
            {
                return iterator(nullptr);
            }

            /**
             * @brief      Returns a constant iterator at the end of the preorder
             *             traversal, i.e at the right of the rightmost branch.
             *
             * @return     the iterator for the end.
             */
            inline const_iterator end() const noexcept
            {
                return iterator(nullptr);
            }

        private:
            T * mValue;
            Node<T>* mParent;
            Node<T>* mRightSibling;
            std::vector<std::unique_ptr<Node<T>>> mChildren;

            void throw_if_out_of_range(size_t child_pos) const
            {
                if (child_pos >= mChildren.size())
                {
                    throw std::invalid_argument("child index is out of bounds");
                }
            }
        };

        /**
         * @brief      Provides preorder traversal functionality for nodes.
         *
         * @tparam     T     the underlying Node type.
         */
        template<typename T>
        class node_iterator
        {
        public:
            node_iterator(Node<T>* current) :
                mCurrent(current)
            {
            }

            /**
             * @brief      Moves to the next Node in the preorder traversal.
             *
             * @return     The next Node
             */
            Node<T>* operator++()
            {
                // case 1 : move to the leftmost child.
                if (mCurrent->child_count() > 0)
                {
                    mCurrent = &(mCurrent->get_child(0));
                }
                // case 2 : move to the right sibling
                else if (mCurrent->right() != nullptr)
                {
                    mCurrent = mCurrent->right();
                }
                // case 3 : move to the first ancestor with a right sibling
                else if (mCurrent->parent() != nullptr)
                {
                    while (mCurrent->right() == nullptr && mCurrent->parent() != nullptr)
                    {
                        mCurrent = mCurrent->parent();
                    }

                    if (mCurrent != nullptr)
                    {
                        mCurrent = mCurrent->right();
                    }
                }
                // case 4 : no suitable ancestor found, return the nullptr,
                // signaling the end of the iteration.
                else
                {
                    mCurrent = nullptr;
                }

                return mCurrent;
            }

            /**
             * @brief      Returns the current Node before moving to the
             *             next Node in the preorder traversal.
             *
             * @return     the Node before the current Node.
             */
            Node<T>* operator++(int)
            {
                auto old = mCurrent;
                ++*this;
                return old;
            }

            /**
             * @brief      Returns the current Node.
             *
             * @return     the current Node.
             */
            inline Node<T>& operator*() const
            {
                return *mCurrent;
            }

            bool operator==(const node_iterator<T>& rhs)
            {
                return mCurrent == rhs.mCurrent;
            }

            bool operator!=(const node_iterator<T>& rhs)
            {
                return mCurrent != rhs.mCurrent;
            }

        private:
            Node<T>* mCurrent;
        };
    }
}

#endif // !ATLAS_GRAPHICS_SCENEGRAPH_HPP

#include "Node.hpp"

namespace atlas
{
    namespace graphics
    {
        Node::Node() :
            _parent(nullptr),
            _rightSibling(nullptr),
            _flags((uint32_t)NodeFlags::None),
            _transform(glm::mat4(1.0)),
            _localTransform(glm::mat4(1.0))
        {
        }

        Node::~Node()
        {
            for (auto child : _children)
            {
                delete child;
            }
        }

        size_t Node::size() const noexcept
        {
            size_t size = 1;
            for (auto const& child : _children)
            {
                size += child->size();
            }
            return size;
        }

        void Node::add_child(Node* child)
        {
            if (child == nullptr)
            {
                throw std::invalid_argument("child is null.");
            }

            child->_parent = this;
            if (!_children.empty())
            {
                _children.back()->_rightSibling = child;
            }
            _children.push_back(child);
        }

        Node* Node::get_child(size_t pos) const
        {
            throw_if_out_of_range(pos);

            return _children[pos];
        }

        void Node::remove_child(size_t pos)
        {
            throw_if_out_of_range(pos);

            _children.erase(_children.begin() + pos);
        }

        void Node::SendSignal(Signal signal)
        {
        }

        void Node::BubbleEvent(SceneGraphEvent event)
        {
            if (_parent != nullptr)
            {
                _parent->BubbleEvent(event);
            }
            else
            {
                _pendingEvents |= 1 << (uint32_t)event;
            }
        }

        void Node::throw_if_out_of_range(size_t child_pos) const
        {
            if (child_pos >= _children.size())
            {
                throw std::invalid_argument("child index is out of bounds");
            }
        }

        NodeIterator Node::begin() noexcept
        {
           return NodeIterator(this);
        }

        NodeIterator Node::end() noexcept
        {
            return NodeIterator(nullptr);
        }

        NodeIterator::NodeIterator(Node* current) :
            _current(current)
        {
        }

        Node* NodeIterator::operator++()
        {
            // case 1 : move to the leftmost child.
            if (_current->child_count() > 0)
            {
                _current = (_current->get_child(0));
            }
            // case 2 : move to the right sibling
            else if (_current->right() != nullptr)
            {
                _current = _current->right();
            }
            // case 3 : move to the first ancestor with a right sibling
            else if (_current->parent() != nullptr)
            {
                while (_current->right() == nullptr && _current->parent() != nullptr)
                {
                    _current = _current->parent();
                }

                if (_current != nullptr)
                {
                    _current = _current->right();
                }
            }
            // case 4 : no suitable ancestor found, return the nullptr,
            // signaling the end of the iteration.
            else
            {
                _current = nullptr;
            }

            return _current;
        }

        Node* NodeIterator::operator++(int)
        {
            auto old = _current;
            ++*this;
            return old;
        }
    }
}
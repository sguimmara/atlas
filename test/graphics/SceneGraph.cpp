#include "lest/lest.hpp"

#include "atlas/graphics/SceneGraph.hpp"

#include <algorithm>


// const std::shared_ptr<int> ROOT = std::make_shared<int>();
// const std::shared_ptr<int> EMPTY = std::shared_ptr<int>();

// class Object
// {
// public:
//     Object(int localMatrix) :
//         m_localMatrix(localMatrix),
//         m_viewMatrix(1)
//     {
//     }

//     int m_localMatrix;
//     int m_viewMatrix;

//     friend std::ostream& operator<< (
//         std::ostream& os,
//         const Object& g)
//     {
//         return os << "(local=" << node.m_localMatrix
//                   << ", view=" << node.m_viewMatrix << ")";
//     }
// };

using namespace atlas::graphics;

const lest::test scene_graph[] =
{
    {
        CASE("passing a nullptr to the ctor throws a std::invalid_argument")
        {
            EXPECT_THROWS_AS(Node<int> node(nullptr), std::invalid_argument);
        }
    },
    {
        CASE("passing a non-null ptr to the ctor")
        {
            int value(3);
            EXPECT_NO_THROW(Node<int> node(&value));
        }
    },
    {
        CASE("dereference operator returns correct value")
        {
            int value1 = 0;
            int value2 = 5;
            Node<int> node(&value1);
            node.add_child(&value2);

            *node = 4;
            node.get_child(0).value() = 55;

            EXPECT(*node == 4);
            EXPECT(*node.get_child(0) == 55);
        }
    },
    {
        CASE("size returns 1 when node has no children")
        {
            int value = 0;
            Node<int> node(&value);
            EXPECT(node.size() == 1);
        }
    },
    {
        CASE("size() returns (n + 1) when node has n children")
        {
            int value = 0;
            Node<int> node(&value);

            size_t child_count = 10;

            for (size_t i = 0; i < child_count; ++i)
            {
                node.add_child(&value);
            }

            EXPECT(node.size() == child_count + 1);
        }
    },
    {
        CASE("parent() returns nullptr for orphan node")
        {
            int value = 0;
            Node<int> node(&value);
            EXPECT(node.parent() == nullptr);
        }
    },
    {
        CASE("parent() returns correct value")
        {
            int value = 0;
            Node<int> node(&value);
            int value2 = 1;
            node.add_child(&value2);
            EXPECT(node.get_child(0).parent() == &node);
        }
    },
    {
        CASE("iterator on no children")
        {
            int value = 0;
            Node<int> node(&value);

            auto it = node.begin();
            EXPECT((*it == node));
        }
    },
    {
        CASE("iterator preincrement and comparison with end()")
        {
            int value = 0;
            Node<int> node(&value);

            auto it = node.begin();
            EXPECT((it != node.end()));
            ++it;
            EXPECT((it == node.end()));
        }
    },
    {
        CASE("iterator postincrement and comparison with end()")
        {
            int value = 0;
            Node<int> node(&value);

            auto it = node.begin();
            EXPECT((it != node.end()));
            it++;
            EXPECT((it == node.end()));
        }
    },
    {
        CASE("remove_child() reduces size")
        {
            int value = 0;
            int child1 = 1;
            int child2 = 2;

            Node<int> node(&value);
            node.add_child(&child1);
            node.add_child(&child2);

            EXPECT_THROWS_AS(node.remove_child(90), std::invalid_argument);
            EXPECT(node.child_count() == 2);
            node.remove_child(0);
            EXPECT(node.child_count() == 1);
            node.remove_child(0);
            EXPECT(node.child_count() == 0);
            EXPECT_THROWS_AS(node.remove_child(0), std::invalid_argument);
        }
    },
    {
        CASE("remove_child() removes correct child")
        {
            int value = 0;
            int child1 = 1;
            int child2 = 2;
            int child3 = 3;
            int child4 = 4;

            Node<int> node(&value);
            node.add_child(&child1);
            node.add_child(&child2);
            node.add_child(&child3);
            node.add_child(&child4);

            node.remove_child(1);
            EXPECT(node.get_child(0).value() == child1);
            EXPECT(node.get_child(1).value() == child3);
            EXPECT(node.get_child(2).value() == child4);
        }
    },
    {
        CASE("the for range loop performs a preorder traversal")
        {
            int value = 0;
            Node<int> node(&value);

            int value1 = 1;
            int value2 = 2;
            int value3 = 3;
            int value4 = 4;
            int value5 = 5;
            int value6 = 6;
            int value7 = 7;
            node.add_child(&value1);
            node.add_child(&value2);
            node.get_child(0).add_child(&value3);
            node.get_child(0).add_child(&value5);
            node.get_child(0).get_child(1).add_child(&value6);
            node.get_child(0).get_child(1).get_child(0).add_child(&value7);
            node.get_child(1).add_child(&value4);

            //    0
            //  1   2
            // 3  5   4
            //    6    <end>
            //    7

            std::vector<int> expected = { 0, 1, 3, 5, 6, 7, 2, 4 };
            std::vector<int> actual;

            for (auto const& n : node)
            {
                actual.push_back(n.value());
            }

            EXPECT(std::equal(actual.begin(), actual.end(), expected.begin()));
        }
    }
};

int main(int argc, char * argv[])
{
    return lest::run(scene_graph, argc, argv);
}
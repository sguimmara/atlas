#include "../../common.hpp"
#include "atlas/core/spatialindex/quadtreenode.hpp"
#include "atlas/core/region.hpp"

using namespace atlas::core;
using namespace atlas::core::spatialindex;

class TestEvaluator : public Evaluator<QuadtreeNode>
{
public:
    TestEvaluator(
        std::function<bool(const QuadtreeNode&)> discard,
        std::function<bool(const QuadtreeNode&)> split) :
        _discard(discard),
        _split(split)
    {
    }

    // returns true if the item must be discarded from the evaluation chain.
    bool discard(const QuadtreeNode& node) const noexcept override
    {
        return _discard(node);
    }

    // returns true if the item must be subdivided.
    // Note that discarded items cannot be subdivided.
    bool subdivide(const QuadtreeNode& node) const noexcept override
    {
        return _split(node);
    }

private:
    std::function<bool(const QuadtreeNode&)> _discard;
    std::function<bool(const QuadtreeNode&)> _split;
};

CASE( "Constructor initializes key and region" "[pass]" )
{
    QuadtreeNode node(Region::world(), QuadtreeNode::Key(9, 5, 2));

    EXPECT(node.key().col() == 9);
    EXPECT(node.key().row() == 5);
    EXPECT(node.key().depth() == 2);
    EXPECT(node.region() == Region::world());
}

CASE("QuadtreeNode::evaluate() does not split root when predicate returns false" "[pass]")
{
    QuadtreeNode node(Region::world(), QuadtreeNode::Key(0, 0, 0));

    TestEvaluator evaluator([](const QuadtreeNode& n) { return false; }, [](const QuadtreeNode& n) { return false; });

    node.evaluate(evaluator);

    std::vector<QuadtreeNode> nodes;
    for (auto& n : node)
    {
        nodes.push_back(n);
    }

    EXPECT(nodes.size() == 1);
    EXPECT(nodes[0].key() == QuadtreeNode::Key(0, 0, 0));
}

CASE("QuadtreeNode::evaluate() for all nodes level 0-1" "[pass]")
{
    QuadtreeNode node(Region::world(), QuadtreeNode::Key(0, 0, 0));

    TestEvaluator evaluator(
        [](const QuadtreeNode& n) { return false; },
        [](const QuadtreeNode& n) { return n.key().depth() < 1; });

    node.evaluate(evaluator);

    std::unordered_map<QuadtreeNode::Key, QuadtreeNode> nodes;
    for (auto& n : node)
    {
        nodes.insert({ n.key(), n });
    }

    EXPECT(nodes.size() == 5);
    EXPECT(nodes.count(QuadtreeNode::Key(0, 0, 0)) == 1);

    EXPECT(nodes.count(QuadtreeNode::Key(0, 0, 1)) == 1);
    EXPECT(nodes.count(QuadtreeNode::Key(1, 0, 1)) == 1);
    EXPECT(nodes.count(QuadtreeNode::Key(1, 1, 1)) == 1);
    EXPECT(nodes.count(QuadtreeNode::Key(0, 1, 1)) == 1);
}
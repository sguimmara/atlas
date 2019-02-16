#include "globeevaluator.hpp"

using namespace atlas::scene;

bool GlobeEvaluator::discard(const QuadtreeNode& node) const noexcept
{
    return false;
}

bool GlobeEvaluator::subdivide(const QuadtreeNode& node) const noexcept
{
    if (node.key().depth() < 2)
    {
        return true;
    }
    else if (node.key().depth() < 4)
    {
        return std::rand() % 3 == 0;
    }
    return false;
}

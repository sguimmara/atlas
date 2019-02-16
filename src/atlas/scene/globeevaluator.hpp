#ifndef ATLAS_SCENE_GLOBEEVALUATOR_HPP
#define ATLAS_SCENE_GLOBEEVALUATOR_HPP

#include "atlas/core/spatialindex/evaluator.hpp"
#include "atlas/core/spatialindex/quadtreenode.hpp"

namespace atlas::scene
{
    using namespace atlas::core::spatialindex;

    class GlobeEvaluator final : public Evaluator<QuadtreeNode>
    {
    public:
        // returns true if the item must be discarded from the evaluation chain.
        bool discard(const QuadtreeNode&) const noexcept override;

        // returns true if the item must be subdivided.
        // Note that discarded items cannot be subdivided.
        bool subdivide(const QuadtreeNode&) const noexcept override;
    };
}

#endif // ATLAS_SCENE_GLOBEEVALUATOR_HPP
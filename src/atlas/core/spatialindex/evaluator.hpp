#ifndef ATLAS_CORE_SPATIALINDEX_EVALUATOR_HPP
#define ATLAS_CORE_SPATIALINDEX_EVALUATOR_HPP

namespace atlas::core::spatialindex
{
    // Base class for evaluators. An evaluator contains predicates that must be
    // called to drive the evaluation of a spatial data structure.
    template<typename T>
    class Evaluator
    {
    public:
        // returns true if the item must be discarded from the evaluation chain.
        virtual bool discard(const T&) const noexcept { return false; }

        // returns true if the item must be subdivided.
        // Note that discarded items cannot be subdivided.
        virtual bool subdivide(const T&) const noexcept = 0;
    };
}

#endif // ATLAS_CORE_SPATIALINDEX_EVALUATOR_HPP
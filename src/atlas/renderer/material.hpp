#ifndef ATLAS_RENDERER_MATERIAL_HPP
#define ATLAS_RENDERER_MATERIAL_HPP

#include "common.hpp"
#include "pipeline.hpp"

namespace atlas::renderer
{
    // the Material represents a particular instance of a pipeline.
    class Material
    {
    public:
        // build a material from a JSON description
        Material(const std::string& description);

    private:
        Pipeline* _pipeline;
    };
}

#endif
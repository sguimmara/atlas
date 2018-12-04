#ifndef ATLAS_RENDERER_ENTITYDATA_HPP
#define ATLAS_RENDERER_ENTITYDATA_HPP

#include "entity_properties.hpp"

namespace atlas::renderer
{
    class EntityData
    {
    public:
        EntityData();
        ~EntityData();
        void update(EntityProperties);

        inline vk::DescriptorSet descriptorSet() const noexcept { return _set; }

    private:
        vk::Buffer _buffer;
        vk::DescriptorSet _set;
    };
}

#endif // ATLAS_RENDERER_ENTITYDATA_HPP
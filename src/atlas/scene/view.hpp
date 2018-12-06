#ifndef ATLAS_SCENE_VIEW_HPP
#define ATLAS_SCENE_VIEW_HPP

#include "common.hpp"
#include "camera.hpp"
#include "atlas/renderer/globalproperties.hpp"
#include "atlas/renderer/uniformbuffer.hpp"

namespace atlas::scene
{
    using namespace atlas::renderer;

    class View
    {
    public:
        View();
        ~View();
        void update();
        inline const UniformBuffer* properties() const noexcept { return _uniformBuffer.get(); }
        inline Camera* camera() noexcept { return _camera.get(); }
        inline const Camera* camera() const noexcept { return _camera.get(); }

    private:
        std::unique_ptr<Camera> _camera;
        GlobalProperties _properties;
        std::unique_ptr<UniformBuffer> _uniformBuffer;
    };
}

#endif
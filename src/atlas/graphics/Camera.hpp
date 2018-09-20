#ifndef ATLAS_GRAPHICS_CAMERA
#define ATLAS_GRAPHICS_CAMERA

#include <spdlog/spdlog.h>

#include "AtlasGraphics.hpp"
#include "Node.hpp"
#include "Renderer.hpp"

namespace atlas
{
    namespace graphics
    {
        class Camera : public Node
        {
        public:
            Camera();

            inline Transform projection() const noexcept { return _projectionMatrix; }
            inline Transform view() const noexcept { return _viewMatrix; }
            void SetFov(float radians);
            inline float fov() const noexcept { return _fovRadians; }
            inline vk::Viewport viewport() const noexcept { return _viewport; }
            inline void setViewport(vk::Viewport viewport) noexcept { _viewport = viewport; }

            void Update(UpdateContext ctx);
            void SendSignal(Signal signal);

            static Camera* current;

        private:
            float _nearClip;
            float _farClip;
            float _aspect;
            float _fovRadians;
            Transform _viewMatrix;
            Transform _projectionMatrix;
            vk::Viewport _viewport;
        };
    }
}

#endif
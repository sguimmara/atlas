#pragma once
#ifndef ATLAS_SCENE_CAMERA_HPP
#define ATLAS_SCENE_CAMERA_HPP

#include "common.hpp"
#include "atlas/renderer/viewport.hpp"
#include "transform.hpp"

namespace atlas::scene
{
    class Camera
    {
    public:
        Camera();
        inline Transform& transform() noexcept { return _transform; }
        void setFov(float degrees);
        inline float fov() const noexcept { return _fov; }
        inline float nearClip() const noexcept { return _nearClip; }
        void setNearClip(float v);
        inline float farClip() const noexcept { return _farClip; }
        void setFarClip(float v);
        inline renderer::Viewport& viewport() noexcept { return _viewport; }
        inline void setViewport(renderer::Viewport viewport) noexcept { _viewport = viewport; }
        mat4 viewMatrix() const noexcept;
        mat4 projectionMatrix() const noexcept;

    private:
        Transform _transform;
        renderer::Viewport _viewport;
        float _fov;
        float _nearClip;
        float _farClip;
    };
}

#endif
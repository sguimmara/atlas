#include "camera.hpp"
#include "atlas/core/constants.hpp"
#include "atlas/renderer/instance.hpp"

using namespace atlas::scene;

Camera::Camera() :
    _viewport({0, 0, 800, 600}),
    _fov(60),
    _nearClip(0.1f),
    _farClip(5.0f)
{}

void Camera::setFov(float degrees)
{
    if (degrees > 0 && degrees <= 150) { _fov = degrees; }
    else throw std::runtime_error("invalid FOV. must be strictly greater than zero degrees and less or equal to 150 degrees.");
}

void Camera::setNearClip(float v)
{
    if (v > 0) { _nearClip = v; }
    else throw std::runtime_error("invalid near clip. Must be strictly greater than zero.");
}

inline void Camera::setFarClip(float v)
{
    if (v > 0) { _farClip = v; }
    else throw std::runtime_error("invalid far clip. Must be strictly greater than zero.");
    if (_farClip < _nearClip) { _nearClip = _farClip - _farClip / 10; }
}

mat4 Camera::viewMatrix() const noexcept
{
    return glm::lookAt(_transform.position(), vec3{ 0,0,0 }, vec3{ 0, -1, 0 });
    // TODO compute proper viewmatrix
    //return glm::inverse(_transform.modelMatrix());
}

mat4 Camera::projectionMatrix() const noexcept
{
    auto screen = atlas::renderer::Instance::context()->viewport();

    float w = screen.width * _viewport.width;
    float h = screen.height * _viewport.height;

    return glm::perspective(glm::radians(_fov), w / h, _nearClip, _farClip);
}

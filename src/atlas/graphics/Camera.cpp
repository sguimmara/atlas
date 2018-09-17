#include "Camera.hpp"
#include "Time.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace atlas
{
    namespace graphics
    {
        Camera* Camera::main = nullptr;

        Camera::Camera() :
            _fovRadians(1),
            _aspect(Renderer::extent.width / static_cast<float>(Renderer::extent.height)),
            _viewport(Renderer::viewport),
            _projectionMatrix(glm::perspective<float>(_fovRadians, _aspect, 0.05f, 15.0f)),
            Node()
        {
            _localTransform = glm::lookAt(glm::vec3(2, -2, -2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

            if (Camera::main == nullptr)
            {
                Camera::main = this;
            }
        }

        void Camera::SetFov(float radians)
        {
            _fovRadians = radians;
            _projectionMatrix = glm::perspective<float>(_fovRadians, _aspect, 0.05f, 15.0f);
        }

        void Camera::Update(UpdateContext ctx)
        {
            //_localTransform = glm::lookAt(glm::vec3(0, 0, -10 + Time::elapsed), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        }

        void Camera::SendSignal(Signal signal)
        {
            if (signal == Signal::WindowResized)
            {
                _aspect = Renderer::extent.width / static_cast<float>(Renderer::extent.height);
                _viewport = Renderer::viewport;
                _projectionMatrix = glm::perspective<float>(_fovRadians, _aspect, 0.05f, 15.0f);
            }
        }
    }
}
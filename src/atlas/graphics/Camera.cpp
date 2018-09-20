#include "Camera.hpp"
#include "Time.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace atlas
{
    namespace graphics
    {
        Camera* Camera::current = nullptr;

        Camera::Camera() :
            _fovRadians(1),
            _aspect(Renderer::extent.width / static_cast<float>(Renderer::extent.height)),
            _viewport(Renderer::viewport),
            _projectionMatrix(glm::perspective<float>(_fovRadians, _aspect, 0.05f, 15.0f)),
            Node()
        {
            _viewMatrix = glm::lookAt(glm::vec3(5, -5, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
            _localTransform = glm::inverse(_viewMatrix);

            if (Camera::current == nullptr)
            {
                Camera::current = this;
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
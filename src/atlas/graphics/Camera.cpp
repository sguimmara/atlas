#include "Camera.hpp"
#include "Time.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace atlas
{
    namespace graphics
    {
        Camera* Camera::main = nullptr;

        Camera::Camera(Renderer* renderer) :
            _renderer(renderer),
            _fovRadians(1),
            _aspect(renderer->extent().width / static_cast<float>(renderer->extent().height)),
            _viewport(renderer->viewport()),
            _projectionMatrix(glm::perspective<float>(_fovRadians, _aspect, 0.05f, 15.0f)),
            Node()
        {
            _localTransform = glm::lookAt(glm::vec3(0, 0, -4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

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
                _aspect = _renderer->extent().width / static_cast<float>(_renderer->extent().height);
                _viewport = _renderer->viewport();
                _projectionMatrix = glm::perspective<float>(_fovRadians, _aspect, 0.05f, 15.0f);
            }
        }
    }
}
#include "transform.hpp"

using namespace atlas::scene;

Transform::Transform() : _modelMatrix(1.0f)
{}

void Transform::move(float x, float y, float z)
{
    _modelMatrix = glm::translate(_modelMatrix, glm::vec3(x, y, z));
}
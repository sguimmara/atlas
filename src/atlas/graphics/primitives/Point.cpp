#include "Point.hpp"

using namespace atlas::graphics::primitives;

Point::Point(glm::vec3 position) : MeshObject(1)
{
    topology = vk::PrimitiveTopology::ePointList;
    std::vector<glm::vec3> positions{ position };
    SetPositions(positions);

    std::vector<glm::vec3> colors{ glm::vec3(0,1,1) };
    SetNormals(colors);
    Apply();
}
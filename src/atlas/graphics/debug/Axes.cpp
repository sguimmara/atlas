#include "Axes.hpp"

using namespace atlas::graphics::debug;

Axes::Axes() :
    Mesh(
        MeshObject(6),
        Material(
            std::vector<Semantic>{ Semantic::Position, Semantic::Color },
            std::vector<Descriptor>(),
            Shader::Get("unlit.vert"),
            Shader::Get("unlit.frag"),
            vk::PrimitiveTopology::eLineList))
{
    _mesh.topology = vk::PrimitiveTopology::eLineList;

    std::vector<glm::vec3> pos;
    pos.resize(6);

    pos[0] = glm::vec3(0, 0, 0);
    pos[1] = glm::vec3(1, 0, 0);

    pos[2] = glm::vec3(0, 0, 0);
    pos[3] = glm::vec3(0, 1, 0);

    pos[4] = glm::vec3(0, 0, 0);
    pos[5] = glm::vec3(0, 0, 1);

    _mesh.SetPositions(pos);
    _mesh.Apply();
}

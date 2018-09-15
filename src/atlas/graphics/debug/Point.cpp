#include "atlas/graphics/AtlasGraphics.hpp"
#include "atlas/graphics/Renderer.hpp"
#include "atlas/graphics/Mesh.hpp"
#include "atlas/graphics/primitives/Primitives.hpp"
#include "Point.hpp"

using namespace atlas::graphics::debug;
namespace prim = atlas::graphics::primitives;

Point::Point(glm::vec3 position) :
    Mesh(
        prim::Point(position),
        Material(
            std::vector<Semantic>{Semantic::Position, Semantic::Color},
            std::vector<Descriptor>(),
            Shader::Get("unlit.vert"),
            Shader::Get("unlit.frag"),
            vk::PrimitiveTopology::ePointList))
{

}
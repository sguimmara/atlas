#include "atlas/renderer/instance.hpp"
#include "atlas/core/quadtree.hpp"
#include "atlas/renderer/pipeline.hpp"
#include "atlas/renderer/mesh.hpp"
#include "atlas/scene/entity.hpp"
#include "atlas/scene/scene.hpp"
#include "atlas/scene/meshbuilder.hpp"
#include "atlas/viewer/viewer.hpp"
#include "atlas/renderer/material.hpp"
#include "atlas/renderer/texture.hpp"
#include "atlas/renderer/globalproperties.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <GLFW/glfw3.h>

using namespace atlas::core;
using namespace atlas::scene;
using namespace atlas::renderer;

int main2(int argc, char* argv[])
{
    atlas::viewer::Viewer viewer;

    atlas::renderer::Texture tex("C:/Users/sguimmara/Pictures/casimir.jpg");

    return 0;
}
int main(int argc, char* argv[])
{
    spdlog::set_level(spdlog::level::trace);

    if (glfwInit() != GLFW_TRUE)
    {
        std::runtime_error("could not initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(800, 600, "test", nullptr, nullptr);

    atlas::renderer::Instance::initialize(window);
    {
        atlas::renderer::Instance::setShaderDirectory("C:/Users/sguimmara/Documents/work/c++/atlas4/.build/bin/shaders/");

        //atlas::renderer::Material mat(R"%(
        //{
        //    "name": "testmat",
        //    "vertex": "default.vert.spv",
        //    "fragment": "solid.frag.spv"
        //})%");

        //atlas::renderer::Material mat2(R"%(
        //{
        //    "name": "with_textures",
        //    "vertex": "default.vert.spv",
        //    "fragment": "default.frag.spv",
        //    "rasterizer": {
        //        "frontFace": "ccw"
        //    }
        //})%");

        //atlas::renderer::Texture casimir("C:/Users/sguimmara/Pictures/casimir.jpg");
        //atlas::renderer::Texture dog("C:/Users/sguimmara/Pictures/dog.jpg");
        //atlas::renderer::Texture dog2("C:/Users/sguimmara/Pictures/dog2.jpg");

        //auto mat3 = mat2.instantiate();
        //auto mat4 = mat2.instantiate();

        //mat2.setTexture("diffuse", &casimir);
        //mat2.setTexture("specular", &casimir);

        //mat3.setTexture("diffuse", &dog);
        //mat3.setTexture("specular", &dog);

        //mat4.setTexture("diffuse", &dog2);
        //mat4.setTexture("specular", &dog2);

        //auto earth = mat2.instantiate();
        //auto earthBg = Texture("C:/Users/sguimmara/Documents/work/c++/atlas/images/Earth.jpg");
        //auto checker = Texture("C:/Users/sguimmara/Documents/work/c++/atlas/images/uv_grid.jpg");
        //earth.setTexture("diffuse", &checker);
        //earth.setTexture("specular", &checker);
        //auto tile = atlas::scene::MeshBuilder::terrain(Region(Cartographic(-0.5,-0.5), 1, 1), 32, Ellipsoid::unitSphere());

        atlas::scene::Scene scene("main");

        //Quadtree quadtree(Region::world(), 8, 4);
        //std::vector<std::unique_ptr<Entity>> tiles;
        //std::vector<std::unique_ptr<Mesh>> meshes;
        //for (auto const& node : quadtree)
        //{
        //    if (node.isleaf())
        //    {
        //        auto mesh = MeshBuilder::terrain(node.region(), 16, Ellipsoid::unitSphere());

        //        auto ent = std::make_unique<Entity>();
        //        ent->mesh = mesh.get();
        //        ent->material = &earth;

        //        meshes.push_back(std::move(mesh));
        //        tiles.push_back(std::move(ent));
        //    }
        //}

        //for (auto const& ent : tiles)
        //{
        //    scene.addEntity(ent.get());
        //}

        //auto const size = 0.3f;

        //std::vector<atlas::renderer::Vertex> vertices =
        //{
        //    atlas::renderer::Vertex{ glm::vec3{0, -size, -size}, glm::vec3{0, 0, 0}, glm::vec2{0, 0} },
        //    atlas::renderer::Vertex{ glm::vec3{0, -size, size}, glm::vec3{0, 0, 0}, glm::vec2{0, 1} },
        //    atlas::renderer::Vertex{ glm::vec3{0, size, size}, glm::vec3{0, 0, 0}, glm::vec2{1, 1} },
        //    atlas::renderer::Vertex{ glm::vec3{0, size, -size}, glm::vec3{0, 0, 0}, glm::vec2{1, 0} }
        //};
        //std::vector<uint16_t> indices = { 0, 2, 1, 0, 3, 2 };

        //atlas::renderer::Mesh triangle(vertices, indices);

        //atlas::scene::Entity triangle1;
        //triangle1.name = "triangle1";
        //triangle1.material = &mat2;
        //triangle1.mesh = &triangle;


        //Entity globe;
        //globe.material = &earth;
        //globe.mesh = tile.get();

        //atlas::scene::Entity triangle2(triangle1);
        //atlas::scene::Entity triangle3(triangle1);
        //atlas::scene::Entity triangle4(triangle1);
        //atlas::scene::Entity triangle5(triangle1);
        //atlas::scene::Entity triangle6(triangle1);
        //triangle1.transform.move(-2, 0, 0);
        //triangle2.transform.move(0, 2, 0);
        //triangle3.transform.move(2, 0, 0);
        //triangle4.transform.move(2, 2, 0);
        //triangle5.transform.move(-2, -2, 0);
        //triangle6.transform.move(2, -2, 0);

        //triangle6.material = &mat3;
        //triangle4.material = &mat4;


        //scene.addEntity(&globe);
        //scene.addEntity(&triangle1);
        //scene.addEntity(&triangle2);
        //scene.addEntity(&triangle3);
        //scene.addEntity(&triangle4);
        //scene.addEntity(&triangle5);
        //scene.addEntity(&triangle6);

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            //triangle1.transform.move(0.01f, 0, 0);
            //triangle2.transform.move(0, -0.01f, 0);
            //triangle3.transform.move(-0.01f, 0, 0);
            //triangle4.transform.move(-0.01f, -0.01f, 0);
            //triangle5.transform.move(0.01f, 0.01f, 0);
            //triangle6.transform.move(-0.01f, 0.01f, 0);

            scene.render();
        }
    }
    uint32_t exitCode = atlas::renderer::Instance::terminate();

    glfwDestroyWindow(window);

    return exitCode;
}
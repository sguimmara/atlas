#include "Atlas.hpp"
#include "atlas/core/Math.hpp"
#include "atlas/graphics/Mesh.hpp"
#include "atlas/graphics/Camera.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

using namespace glm;
using namespace atlas::graphics;
using namespace atlas::core;

atlas::Atlas::Atlas()
{
    _log = spdlog::stdout_color_mt("atlas");

    _log->info("started");
#if DEBUG
    _log->set_level(spdlog::level::trace);
    _log->info("configuration is Debug");
#endif

    InitWindow();

    _renderer = new Renderer();

    _renderer->Setup(_window);

    _scene = new Scene();

    auto cyan = vec3(0, 1, 1);
    auto red = vec3(1, 0, 0);
    auto green = vec3(0, 1, 0);
    auto blue = vec3(0, 0, 1);
    auto gray = vec3(0.5f, 0.5f, 0.5f);
    auto lightRed = vec3(0.7f, 0.2f, 0.2f);
    auto black = vec3(0, 0, 0);

    Ellipsoid grs80 = Ellipsoid::GRS80;
    Node* ecef = new Node();
    Mesh point = Mesh::MakePoint(cyan, vec3(0, 0, 0));
    Mesh xAxis = Mesh::MakeLine(red, vec3(0, 0, 0), vec3(1, 0, 0));
    Mesh yAxis = Mesh::MakeLine(green, vec3(0, 0, 0), vec3(0, 1, 0));
    Mesh zAxis = Mesh::MakeLine(blue, vec3(0, 0, 0), vec3(0, 0, 1));
    Mesh plane = Mesh::MakePlane(gray);
    plane.setLocalTransform(glm::scale(plane.localTransform(), vec3(4, 4, 4)));
    Mesh equator = Mesh::MakeParallel(cyan, 0, grs80);
    Mesh zeroMeridian = Mesh::MakeMeridian(green, 0, grs80);
    Mesh northernTropic = Mesh::MakeParallel(lightRed, Math::ToRadians(23.43686), grs80);
    Mesh southernTropic = Mesh::MakeParallel(lightRed, Math::ToRadians(-23.43686), grs80);
    Mesh ellipsoid = Mesh::MakeSolidEllipsoid(gray, 20, grs80);
    Mesh grid = Mesh::MakeEllipsoid(black, 20, grs80);

    vec2 min = vec2(0.1, 0.1);
    Mesh region = Mesh::MakeRegion(cyan, min, vec2{ min.x + 0.3f, min.y + 0.3f }, grs80);

    const float EcefToVulkan[] = {
         0, 1, 0, 0,
         0, 0,-1, 0,
        -1, 0, 0, 0,
         0, 0, 0, 1
    };

    const float VulkanToEcef[] = {
         0, 0,-1, 0,
         1, 0, 0, 0,
         0,-1, 0, 0,
         0, 0, 0, 1
    };

    ecef->setLocalTransform(make_mat4(VulkanToEcef));

    _scene->root()->add_child(ecef);
    ecef->add_child(&point);
    ecef->add_child(&xAxis);
    ecef->add_child(&yAxis);
    ecef->add_child(&zAxis);
    ecef->add_child(&plane);
    ecef->add_child(&ellipsoid);
    ecef->add_child(&grid);
    ecef->add_child(&zeroMeridian);
    ecef->add_child(&equator);
    ecef->add_child(&northernTropic);
    ecef->add_child(&southernTropic);
    ecef->add_child(&region);
    //_scene->root()->add_child(new Earth());
    _scene->root()->add_child(new Camera());
    _renderer->SetScene(_scene);

    glfwSetWindowUserPointer(_window, _renderer);

    _log->info("scene created");

    _renderer->Run();
}

/* initializes the GLFW window */
void atlas::Atlas::InitWindow()
{
    if (glfwInit() != GLFW_TRUE)
    {
        _log->critical("GLFW failed to initialize.");
        std::terminate();
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (!(_window = glfwCreateWindow(800, 600, APP_NAME, nullptr, nullptr)))
    {
        throw std::runtime_error("GLFW failed to create window");
    }

    _log->debug("initialized window");
}

#include "scene.hpp"
#include "atlas/renderer/instance.hpp"
#include "atlas/renderer/pipeline.hpp"

using namespace atlas::scene;

Scene::Scene(std::string name) : _name(name)
{
    auto const logName = "scene <" + name + ">";
    _log = spdlog::get(logName);
    if (!_log)
    {
        _log = spdlog::stdout_color_mt(logName);
    }
    _log->info("initialized");

    _globe = std::make_unique<Globe>();
}

void Scene::updateGlobalProperties()
{
    _properties.viewMatrix = glm::lookAt(glm::vec3{ 0, 0, 3 }, glm::vec3{ 0,0,0 }, glm::vec3{ 0, -1, 0 });
    auto const viewport = renderer::Instance::context()->viewport();
    _properties.projMatrix = glm::perspective<float>(1, viewport.width / (float)viewport.height, 0.2f, 10.0f);
    _properties.viewportTime = glm::vec4(viewport.width, viewport.height, 0, 0);
    _properties.sunDirection = glm::vec4(-1, 1, -1, 0);
    _properties.sunColorAndIntensity = glm::vec4(1, 1, 0.9, 2);
}

Scene::~Scene()
{
    _log->info("destroyed");
}

void Scene::addEntity(Entity* entity)
{
    _entities.push_back(entity);
    _log->debug("entity '{0}' added", entity->name);
}

void Scene::render()
{
    auto ctx = renderer::Instance::context();
    if (!ctx)
    {
        return;
    }

    updateGlobalProperties();

    for (auto const& entity : _entities)
    {
        entity->update();
    }

    ctx->beginFrame();
    ctx->bind(_properties);

    for (auto const& tile : _globe->tiles())
    {
        ctx->bind(tile->material->pipeline());
        ctx->draw(tile->data()->descriptorSet(), tile->material->descriptorSet(), *tile->mesh);
    }

    //for (auto const& entity : _entities)
    //{
    //    ctx->bind(entity->material->pipeline());
    //    ctx->draw(entity->data()->descriptorSet(), entity->material->descriptorSet(), *entity->mesh);
    //}
    ctx->endFrame();
}

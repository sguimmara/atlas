#include "scene.hpp"

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

    _globe = std::make_unique<Globe>(Ellipsoid::unitSphere());

    View* front = new View();
    auto frontCam = front->camera();
    frontCam->transform().move(0, 0, 3);
    frontCam->viewport().height = 1;
    frontCam->viewport().width = 0.5f;
    _views.push_back(std::unique_ptr<View>(front));

    View* side = new View();
    auto sideCam = side->camera();
    sideCam->transform().move(3, 0, 0);
    sideCam->viewport().height = 1;
    sideCam->viewport().width = 0.5f;
    sideCam->viewport().x = 0.5f;
    _views.push_back(std::unique_ptr<View>(side));
}

Scene::~Scene()
{
    _log->info("destroyed");
}

void Scene::render()
{
    auto ctx = renderer::Instance::context();
    if (!ctx)
    {
        return;
    }

    ctx->beginFrame();
    {
        for (auto const& view : _views)
        {
            setupView(*view);

            renderGlobe(ctx, *view);
        }
    }
    ctx->endFrame();
}

void Scene::renderGlobe(atlas::renderer::Context* ctx, View& view)
{
    _globe->update();

    for (auto const& tile : _globe->tiles())
    {
        ctx->bind(tile->material().pipeline());
        ctx->draw(
            view.properties()->descriptorSet(),
            tile->data()->descriptorSet(),
            tile->material().descriptorSet(),
            tile->mesh());
    }
}

void Scene::setupView(View& view)
{
    view.update();

    auto const viewport = view.camera()->viewport();

    // TODO move logic into context
    auto const screen = Instance::context()->viewport();
    vk::Viewport screenSpaceViewport = {
        viewport.x * screen.width,
        viewport.y * screen.height,
        viewport.width * screen.width,
        viewport.height * screen.height };

    Instance::context()->setViewport(screenSpaceViewport);
}

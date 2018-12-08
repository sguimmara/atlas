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

    View* side = new View();
    side->camera()->setViewport(Viewport{ 0.0f, 0.0f, 1.0f, 1.0f });
    side->camera()->transform().move(0, 0, 2.5f);
    _views.push_back(std::unique_ptr<View>(side));
}

Scene::~Scene()
{
    _log->info("destroyed");
}

void Scene::render(const FrameInfo& frameInfo)
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

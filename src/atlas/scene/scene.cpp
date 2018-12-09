#include "scene.hpp"
#include "meshbuilder.hpp"

using namespace atlas::scene;

Scene::Scene(std::string name) :
    _name(name),
    _srs(std::make_unique<Ellipsoid>(Ellipsoid::unitSphere()))
    //_srs(std::make_unique<Equirectangular>())
{
    auto const logName = "scene <" + name + ">";
    _log = spdlog::get(logName);
    if (!_log)
    {
        _log = spdlog::stdout_color_mt(logName);
    }
    _log->info("initialized");

    _globe = std::make_unique<Globe>(_srs.get());

    View* side = new View();
    side->camera()->setViewport(Viewport{ 0.0f, 0.0f, 1.0f, 1.0f });
    side->camera()->transform().move(2, 2, 0);
    _views.push_back(std::unique_ptr<View>(side));

    //auto vertexColor = Pipeline::create(R"%(
    //    {
    //        "name": "vertexcolor",
    //        "vertex": "default.vert.spv",
    //        "fragment": "vertexcolor.frag.spv",
    //        "rasterizer":{
    //            "cullMode": "none",
    //            "frontFace": "ccw"
    //        }
    //    })%");

    //Entity* tripod = new Entity(std::make_shared<Material>(vertexColor), MeshBuilder::tripodPyramid());
    //_entities.push_back(std::unique_ptr<Entity>(tripod));
}

Scene::~Scene()
{
    _log->info("destroyed");
}

void Scene::render(const Time& time)
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
            setupView(*view, time);

            renderGlobe(ctx, *view);
            for (auto const& ent : _entities)
            {
                drawEntity(ctx, *ent, *view);
            }
        }
    }
    ctx->endFrame();
}

void Scene::renderGlobe(atlas::renderer::Context* ctx, View& view)
{
    _globe->update();

    for (auto const& tile : _globe->tiles())
    {
        drawEntity(ctx, *tile, view);
    }
}

void Scene::drawEntity(atlas::renderer::Context* ctx, const Entity& entity, View& view)
{
    ctx->bind(entity.material().pipeline());
    ctx->draw(
        view.properties()->descriptorSet(),
        entity.data()->descriptorSet(),
        entity.material().descriptorSet(),
        entity.mesh());
}

void Scene::setupView(View& view, const Time& time)
{
    view.update(time);

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

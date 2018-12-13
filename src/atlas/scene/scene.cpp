#include "scene.hpp"
#include "meshbuilder.hpp"
#include "geographicgrid.hpp"

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

    //// TODO: inject
    _TMP_ImageSource = std::make_unique<FileImageSource>(Region::world(), "C:/Users/sguimmara/Documents/work/c++/atlas/images/blue_marble.jpg");

    _layers.push_back(std::make_unique<Globe>(_srs.get()));
    _layers.push_back(std::make_unique<GeographicGrid>(_srs.get()));

    View* side = new View();
    side->camera()->setViewport(Viewport{ 0.0f, 0.0f, 1.0f, 1.0f });
    side->camera()->transform().move(2, 2, 0);
    _views.push_back(std::unique_ptr<View>(side));

    globe()->setImageSource(_TMP_ImageSource.get());
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

            for (auto const& layer : _layers)
            {
                if (layer->active())
                {
                    layer->update();
                    for (auto const& entity : layer->entities())
                    {
                        //drawEntity(ctx, *entity, *view);
                    }
                    for (auto const& entity : layer->debugEntities())
                    {
                        drawEntity(ctx, *entity, *view);
                    }
                }
            }
        }
    }
    ctx->endFrame();
}

Globe* Scene::globe() const noexcept
{
    return static_cast<Globe*>(getLayer("globe"));
}

Layer* atlas::scene::Scene::getLayer(const std::string& name) const
{
    for (auto const& layer : _layers)
    {
        if (layer->name() == name)
        {
            return layer.get();
        }
    }
    return nullptr;
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

    Instance::context()->setViewport(view.camera()->viewport());
}

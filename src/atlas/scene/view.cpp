#include "view.hpp"
#include "atlas/renderer/allocator.hpp"
#include "atlas/renderer/instance.hpp"

using namespace atlas::scene;

View::View() :
    _camera(std::make_unique<Camera>()),
    _uniformBuffer(Instance::getGlobalPropertyBuffer())
{}

View::~View()
{
}

void View::update()
{
    _properties.viewMatrix = _camera->viewMatrix();
    _properties.projMatrix = _camera->projectionMatrix();
    _properties.sunDirection = vec4(-1, 1, -1, 0);
    _properties.sunColorAndIntensity = vec4(1, 1, 0.9, 2);

    _uniformBuffer->update(&_properties);
}
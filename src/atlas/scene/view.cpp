#include "view.hpp"
#include "atlas/renderer/allocator.hpp"
#include "atlas/renderer/instance.hpp"

#include <iostream>

using namespace atlas::scene;

View::View() :
    _camera(std::make_unique<Camera>()),
    _uniformBuffer(Instance::getGlobalPropertyBuffer())
{}

View::~View()
{
}

void View::update(const Time& time)
{
    _properties.viewMatrix = _camera->viewMatrix();
    _properties.projMatrix = _camera->projectionMatrix();
    double x = std::cos(time.elapsed.count() / 1000.0);
    double y = std::sin(time.elapsed.count() / 1000.0);
    _properties.sunDirection = vec4(x, y, 0, 0);
    _properties.sunColorAndIntensity = vec4(1, 1, 0.8, 1.4f);

    _uniformBuffer->update(&_properties);
}
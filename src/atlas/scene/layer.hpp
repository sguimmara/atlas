#ifndef ATLAS_SCENE_LAYER_HPP
#define ATLAS_SCENE_LAYER_HPP

#include <string>
#include "entity.hpp"

namespace atlas::scene
{
    // The Layer is the fundamental subdivision of a scene.
    // Each layers handles a thematic : terrain, vectors...
    // Moreover, a layer can be enabled or disabled.
    class Layer
    {
    public:
        Layer(std::string name) : _name(name) {}
        virtual ~Layer() {}

        inline std::string name() const noexcept { return _name; }

        virtual std::vector<const Entity*> entities() const {
            return std::vector<const Entity*>();
        }

        virtual std::vector<const Entity*> debugEntities() const {
            return std::vector<const Entity*>();
        }

        virtual void update() {}

        // determines whether the layer should be drawn or not.
        inline bool active() const noexcept { return _active; }
        inline void setActive(bool v) noexcept { _active = v; }

    private:
        bool _active;
        std::string _name;
    };
}

#endif // ATLAS_SCENE_LAYER_HPP
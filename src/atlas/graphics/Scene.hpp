#ifndef ATLAS_GRAPHICS_SCENE
#define ATLAS_GRAPHICS_SCENE

#include <spdlog/spdlog.h>
#include <list>

#include "Node.hpp"

namespace atlas
{
    namespace graphics
    {
        class Camera;
        class Mesh;

        class Scene
        {
        public:
            Scene();
            ~Scene();

            inline Node* root() const noexcept { return _root; }

            void ProcessEvents();

            std::vector<Camera*> cameras();

            /* Drawables ordered by material to minimize draw calls */
            inline std::list<Mesh*> renderList() noexcept { return _renderList; };

            inline NodeIterator begin() { return _root->begin(); }
            inline NodeIterator end() { return _root->end(); }

        private:
            Node* _root;
            std::list<Mesh*> _renderList;
        };
    }
}

#endif
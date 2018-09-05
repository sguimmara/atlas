#ifndef ATLAS_GRAPHICS_SCENE
#define ATLAS_GRAPHICS_SCENE

#include <spdlog/spdlog.h>

#include "Node.hpp"

namespace atlas
{
    namespace graphics
    {
        class Camera;

        class Scene
        {
        public:
            Scene();
            ~Scene()
            { 
                for (auto node : *_root)
                {
                    delete node;
                }
            }
            inline Node* root() const noexcept { return _root; }

            std::vector<Camera*> cameras();

            inline NodeIterator begin() { return _root->begin(); }
            inline NodeIterator end() { return _root->end(); }

        private:
            Node* _root;
        };
    }
}

#endif
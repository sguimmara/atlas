#include "Scene.hpp"
#include "Camera.hpp"

namespace atlas
{
    namespace graphics
    {
        Scene::Scene() :
            _root(new Node())
        {
        }

        std::vector<Camera*> Scene::cameras()
        {
            std::vector<Camera*> result;

            for (auto node : *_root)
            {
                Camera* cam = dynamic_cast<Camera*>(node);
                if (cam != nullptr)
                {
                    result.push_back(cam);
                }
            }

            return result;
        }
    }
}

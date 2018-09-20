#include "Scene.hpp"
#include "Camera.hpp"

namespace atlas
{
    namespace graphics
    {
        Scene::Scene() :
            _root(new Node())
        {
            //float ecefToVulkan[] = {
            //     0, 1, 0, 0,
            //     0, 0,-1, 0,
            //    -1, 0, 0, 0,
            //     0, 0, 0, 1
            //};

            // _root->setTransform(make_mat4(ecefToVulkan));
        }

        Scene::~Scene()
        {
            delete _root;
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

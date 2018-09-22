#include "Scene.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"

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

        bool sortMeshes(const Mesh* first, const Mesh* second)
        {
            return first->material()->materialID < second->material()->materialID;
        }

        void Scene::ProcessEvents()
        {
            if (_root->pendingEvents() == 0)
            {
                return;
            }

            auto pending = _root->pendingEvents();
            if (pending & 1 << (uint32_t)SceneGraphEvent::MeshAdded ||
                pending & 1 << (uint32_t)SceneGraphEvent::MeshDeleted)
            {
                _renderList.clear();
                for (auto node : *_root)
                {
                    Mesh* mesh = dynamic_cast<Mesh*>(node);
                    if (mesh != nullptr)
                    {
                        _renderList.push_back(mesh);
                    }
                }
                _renderList.sort(sortMeshes);
            }

            _root->ClearPendingEvents();
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

#ifndef ATLAS_GRAPHICS_MESH_HPP
#define ATLAS_GRAPHICS_MESH_HPP

#include "AtlasGraphics.hpp"
#include "Node.hpp"
#include "Drawable.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "MeshObject.hpp"
#include "Material.hpp"
#include "Image.hpp"

namespace atlas
{
    namespace graphics
    {
        /**
        * @brief A Mesh is a Drawable with vertices organized in a mesh
        */
        class Mesh : public Drawable
        {
        public:
            Mesh(MeshObject mesh, Material material);
            ~Mesh();

            /**
            * @brief Fills the command buffer with draw commands
            */
            void Draw(DrawContext context);

            void SendSignal(Signal signal);


        protected:
            Material _material;
            MeshObject _mesh;
            glm::vec3 _color;
        };
    }
}

#endif
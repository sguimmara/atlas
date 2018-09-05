#include "lest/lest.hpp"

#include "atlas/core/Atlas.hpp"
#include "atlas/graphics/AtlasGraphics.hpp"
#include "atlas/graphics/Renderer.hpp"

using namespace atlas::graphics;

const lest::test specification[] =
{
    {
        CASE("renderer")
        {
            //GLFWwindow* window;

            //EXPECT(glfwInit() == GLFW_TRUE);

            //glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            //if (!(window = glfwCreateWindow(800, 600, APP_NAME, nullptr, nullptr)))
            //{
            //    throw std::runtime_error("GLFW failed to create window");
            //}

            //Renderer r;
            //r.Setup(window);

            //Scene* scene = new Scene();

            atlas::Atlas app;

            //r.SetScene(scene);
            //r.Run();
        }
    },

};

int main(int argc, char * argv[])
{
    return lest::run(specification, argc, argv);
}

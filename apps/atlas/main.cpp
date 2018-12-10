#include <tclap/CmdLine.h>

#include "atlas/viewer/viewer.hpp"
#include "atlas/viewer/parameters.hpp"

int main(int argc, char** argv)
{
    try
    {
        TCLAP::CmdLine cmd(APP_NAME, ' ', APP_VERSION);

        TCLAP::ValueArg<std::string> shaderDirectory("", "shader-directory", "path to the shader directory", true, ".", "string");

        cmd.add(shaderDirectory);

        cmd.parse(argc, argv);

        atlas::viewer::Parameters params = {};
        params.shaderDirectory = shaderDirectory;

        atlas::viewer::Viewer viewer(params);

        viewer.run();
    }
    catch (TCLAP::ArgException &e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

    return 0;
}
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

        TCLAP::MultiSwitchArg verbosity("v", "verbose", "increase verbosity of log output", 1);
        cmd.add(verbosity);

        TCLAP::SwitchArg quiet("q", "quiet", "suppress log output (except errors)");
        cmd.add(quiet);

        cmd.parse(argc, argv);

        atlas::viewer::Parameters params = {};
        params.shaderDirectory = shaderDirectory;
        params.verbosity = verbosity.getValue();
        if (quiet.getValue())
        {
            params.verbosity = 0;
        }

        atlas::viewer::Viewer viewer(params);

        viewer.run();
    }
    catch (TCLAP::ArgException &e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

    return 0;
}
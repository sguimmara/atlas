#include <tclap/CmdLine.h>

int main(int argc, char** argv)
{
    try
    {
        TCLAP::CmdLine cmd(APP_NAME, ' ', APP_VERSION);

        TCLAP::ValueArg<std::string> shaderDirectory("", "shader-directory", "path to the shader directory", true, "homer", "string");
        TCLAP::ValueArg<std::string> cacheDirectory("", "cache-directory", "path to the cache directory", true, "homer", "string");

        cmd.add(shaderDirectory);
        cmd.add(cacheDirectory);

        // Parse the argv array.
        cmd.parse(argc, argv);
    }
    catch (TCLAP::ArgException &e)  // catch any exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

    return 0;
}
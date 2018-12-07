#include <tclap/CmdLine.h>

int main(int argc, char** argv)
{
    // Wrap everything in a try block.  Do this every time,
        // because exceptions will be thrown for problems.
    try {

        // Define the command line object, and insert a message
        // that describes the program. The "Command description message"
        // is printed last in the help text. The second argument is the
        // delimiter (usually space) and the last one is the version number.
        // The CmdLine object parses the argv array based on the Arg objects
        // that it contains.
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
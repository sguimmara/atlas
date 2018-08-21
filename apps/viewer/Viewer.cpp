#include "atlas/core/Atlas.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    atlas::Atlas app;


    std::cout << "press any key to exit.." << std::endl;
    std::getc(stdin);

    return 0;
}
#include "lest/lest.hpp"

#define CASE( name ) lest_CASE( specification(), name )

lest::tests & specification()
{
    static lest::tests tests;
    return tests;
}

int main( int argc, char * argv[] )
{
    return lest::run( specification(), argc, argv);
}
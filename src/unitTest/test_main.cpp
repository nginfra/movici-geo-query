#define CATCH_CONFIG_RUNNER
#include <unitTest/catch.hpp>

int main( int argc, char* argv[] )
{
    int result = Catch::Session().run( argc, argv );

    return (result < 0xff ? result : 0xff);
}

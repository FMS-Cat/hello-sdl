cmake_minimum_required( VERSION 3.8 )

include( FetchContent )

set( SDL2_VERSION "2.0.12" )

# SDL2
FetchContent_Declare( external_sdl2
    URL http://www.libsdl.org/release/SDL2-${SDL2_VERSION}.tar.gz
)
FetchContent_MakeAvailable( external_sdl2 )

project( "hello-sdl" )
add_subdirectory( "hello-sdl" )

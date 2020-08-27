cmake_minimum_required( VERSION 3.8 )

include( FetchContent )

set( SDL2_VERSION "2.0.12" )
set( ROCKET_TAG "master" )

# SDL2
FetchContent_Declare( external_sdl2
    URL http://www.libsdl.org/release/SDL2-${SDL2_VERSION}.tar.gz
)

FetchContent_GetProperties( external_sdl2 )
if( NOT external_sdl2_POPULATED )
  FetchContent_Populate( external_sdl2 )
  add_subdirectory( ${external_sdl2_SOURCE_DIR} ${external_sdl2_BINARY_DIR} EXCLUDE_FROM_ALL )
endif()

# Rocket
FetchContent_Declare( external_rocket
    GIT_REPOSITORY git@github.com:emoon/rocket.git
    GIT_TAG ${ROCKET_TAG}
    BUILD_COMMAND "make -j8 lib/librocket.a"
)

FetchContent_GetProperties( external_rocket )
if( NOT external_rocket_POPULATED )
  FetchContent_Populate( external_rocket )
  add_subdirectory( ${external_rocket_SOURCE_DIR} ${external_rocket_BINARY_DIR} EXCLUDE_FROM_ALL )
endif()

# hello-sdl
project( "hello-sdl" )
add_subdirectory( "hello-sdl" )

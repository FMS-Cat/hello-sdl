cmake_minimum_required( VERSION 3.8 )

add_executable( hello-sdl hello-sdl.cpp )

if(WIN32)
  target_link_libraries( hello-sdl PRIVATE wsock32 ws2_32 )
endif()

target_link_libraries( hello-sdl PRIVATE SDL2 SDL2main rkt_sync )

target_include_directories( hello-sdl PRIVATE SDL2 rkt_sync )

add_custom_command(
    TARGET
    hello-sdl
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${external_sdl2_BINARY_DIR}/$<CONFIG>/SDL2d.dll"
    $<TARGET_FILE_DIR:hello-sdl>
)

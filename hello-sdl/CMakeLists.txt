cmake_minimum_required( VERSION 3.8 )

add_executable( hello-sdl "hello-sdl.cpp" )

target_link_libraries( hello-sdl SDL2 SDL2main )

target_include_directories( hello-sdl PRIVATE SDL2 )

add_custom_command(
    TARGET
    hello-sdl
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${external_sdl2_BINARY_DIR}/$<CONFIG>/SDL2d.dll"
    $<TARGET_FILE_DIR:hello-sdl>
)

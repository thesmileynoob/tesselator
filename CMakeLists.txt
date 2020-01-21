cmake_minimum_required(VERSION 3.0.0)
project(platformer VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-g -Wall -std=c11)
add_compile_options(-Werror)

include_directories('/usr/include/SDL2' '-D_REENTRANT')

add_executable(debug
        main.c
        player.c
        tile.c
        step.c
        physics.c

        scratchpad.c
)

target_link_libraries(debug SDL2 SDL2_image)

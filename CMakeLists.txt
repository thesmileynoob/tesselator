cmake_minimum_required(VERSION 3.0.0)
project(platformer VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-g -Wall -std=c++17)
#add_compile_options(-Werror)
#add_compile_options(-Wextra)

include_directories('/usr/include/SDL2' '-D_REENTRANT')

add_executable(debug
        main.cc
        object.cc
        player.cc
        ball.cc
        tile.cc
        ui.cc
        math.cc
        scratchpad.cc
)

target_link_libraries(debug m SDL2 SDL2_image SDL2_ttf)

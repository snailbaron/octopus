add_library(sdl
    error.cpp
    sdl.cpp
)
target_include_directories(sdl PUBLIC include)
target_link_libraries(sdl
    PUBLIC
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
)
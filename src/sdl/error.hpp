#pragma once

#include "sdl/error.hpp"

#include <SDL.h>
#include <SDL_image.h>

namespace sdl {

void check(int errorCode);

template <class T>
T* check(T* ptr)
{
    if (ptr == nullptr) {
        throw Error{SDL_GetError()};
    }
    return ptr;
}

} // namespace sdl

namespace img {

template <class T>
T* check(T* ptr)
{
    if (ptr == nullptr) {
        throw sdl::Error{IMG_GetError()};
    }
    return ptr;
}

} // namespace img
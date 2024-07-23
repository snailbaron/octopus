#include "sdl.hpp"

#include "error.hpp"

#include <iostream>

namespace sdl {

Init::Init(uint32_t flags)
{
    check(SDL_Init(flags));
}

Init::~Init()
{
    SDL_Quit();
}

Surface::Surface(SDL_Surface* ptr)
{
    _ptr.reset(ptr);
}

Texture::Texture(SDL_Texture* ptr)
{
    _ptr.reset(ptr);
}

Size Texture::size()
{
    auto size = Size{};
    check(SDL_QueryTexture(ptr(), nullptr, nullptr, &size.w, &size.h));
    return size;
}

Window::Window(const char* title, int x, int y, int w, int h, uint32_t flags)
{
    _ptr.reset(check(SDL_CreateWindow(title, x, y, w, h, flags)));
}

Size Window::size()
{
    auto size = Size{};
    SDL_GetWindowSize(ptr(), &size.w, &size.h);
    return size;
}

Renderer::Renderer(Window& window, int index, uint32_t flags)
{
    _ptr.reset(check(SDL_CreateRenderer(window.ptr(), index, flags)));
}

Texture Renderer::createTextureFromSurface(Surface& surface)
{
    return Texture{check(SDL_CreateTextureFromSurface(ptr(), surface.ptr()))};
}

Texture Renderer::loadTexture(const std::filesystem::path& file)
{
    return Texture{check(IMG_LoadTexture(ptr(), file.string().c_str()))};
}

Texture Renderer::loadTexture(void* mem, int size)
{
    auto rwops = RWops{mem, size};
    return Texture{check(IMG_LoadTexture_RW(ptr(), rwops.ptr(), 0))};
}

void Renderer::copy(
    Texture& texture, const SDL_Rect& srcrect, const SDL_FRect& dstrect)
{
    check(SDL_RenderCopyF(ptr(), texture.ptr(), &srcrect, &dstrect));
}

void Renderer::setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    check(SDL_SetRenderDrawColor(ptr(), r, g, b, a));
}

void Renderer::clear()
{
    check(SDL_RenderClear(ptr()));
}

void Renderer::present()
{
    SDL_RenderPresent(ptr());
}

RWops::RWops(const std::filesystem::path& file, const char* mode)
{
    _ptr.reset(check(SDL_RWFromFile(file.string().c_str(), mode)));
}

RWops::RWops(void* mem, int size)
{
    _ptr.reset(check(SDL_RWFromMem(mem, size)));
}

RWops::RWops(const void* mem, int size)
{
    _ptr.reset(check(SDL_RWFromConstMem(mem, size)));
}

std::optional<SDL_Event> pollEvent()
{
    auto event = SDL_Event{};
    if (SDL_PollEvent(&event)) {
        return event;
    }
    return std::nullopt;
}

} // namespace sdl

namespace img {

Init::Init(int flags)
{
    if (IMG_Init(flags) != flags) {
        throw sdl::Error{"failed to initialize SDL2_image"};
    }
}

Init::~Init()
{
    IMG_Quit();
}

sdl::Surface load(const std::filesystem::path& file)
{
    return sdl::Surface{check(IMG_Load(file.string().c_str()))};
}

sdl::Surface loadRW(sdl::RWops src)
{
    return sdl::Surface{check(IMG_Load_RW(src.ptr(), 0))};
}

} // namespace img

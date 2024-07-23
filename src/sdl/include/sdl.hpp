#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>

namespace helper {

template <class WrappedType, void (*Destroyer)(WrappedType*)>
class Wrapper {
public:
    WrappedType* ptr()
    {
        return _ptr.get();
    }

    const WrappedType* ptr() const
    {
        return _ptr.get();
    }

    WrappedType* operator->()
    {
        return _ptr.get();
    }

    const WrappedType* operator->() const
    {
        return _ptr.get();
    }

protected:
    std::unique_ptr<WrappedType, void (*)(WrappedType*)> _ptr{
        nullptr, Destroyer};
};

} // namespace helper

namespace sdl {

class [[nodiscard]] Init {
public:
    explicit Init(uint32_t flags);
    ~Init();

    Init(const Init&) = delete;
    Init(Init&&) = delete;
    Init& operator=(const Init&) = delete;
    Init& operator=(Init&&) = delete;
};

class Surface : public helper::Wrapper<SDL_Surface, SDL_FreeSurface> {
public:
    explicit Surface(SDL_Surface* ptr);
};

struct Size {
    int w = 0;
    int h = 0;
};

class Texture : public helper::Wrapper<SDL_Texture, SDL_DestroyTexture> {
public:
    explicit Texture(SDL_Texture* ptr);

    Size size();
};

class Window : public helper::Wrapper<SDL_Window, SDL_DestroyWindow> {
public:
    Window(const char* title, int x, int y, int w, int h, uint32_t flags);

    Size size();
};

class Renderer : public helper::Wrapper<SDL_Renderer, SDL_DestroyRenderer> {
public:
    Renderer(Window& window, int index, uint32_t flags);

    Texture createTextureFromSurface(Surface& surface);

    Texture loadTexture(const std::filesystem::path& file);
    Texture loadTexture(void* mem, int size);

    void
    copy(Texture& texture, const SDL_Rect& srcrect, const SDL_FRect& dstrect);

    void setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    void clear();
    void present();
};

class RWops : public helper::Wrapper<SDL_RWops, [](SDL_RWops* ops) {
    SDL_RWclose(ops);
}> {
public:
    RWops(const std::filesystem::path& file, const char* mode);
    RWops(void* mem, int size);
    RWops(const void* mem, int size);
};

std::optional<SDL_Event> pollEvent();

} // namespace sdl

namespace img {

class Init {
public:
    explicit Init(int flags);
    ~Init();

    Init(const Init&) = delete;
    Init(Init&&) = delete;
    Init& operator=(const Init&) = delete;
    Init& operator=(Init&&) = delete;
};

sdl::Surface load(const std::filesystem::path& file);
sdl::Surface loadRW(sdl::RWops src);

} // namespace img

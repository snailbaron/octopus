#include "error.hpp"

#include "error.hpp"
#include <format>

namespace sdl {

void check(int errorCode)
{
    if (errorCode != 0) {
        throw Error{SDL_GetError()};
    }
}

Error::Error(const char* message, std::source_location sl)
    : _message(std::format(
        "{}: {}:{} ({}): {}",
        sl.file_name(),
        sl.line(),
        sl.column(),
        sl.function_name(),
        message))
{ }

const char* Error::what() const noexcept
{
    return _message.c_str();
}

} // namespace sdl
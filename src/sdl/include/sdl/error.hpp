#pragma once

#include <exception>
#include <source_location>
#include <string>

namespace sdl {

class Error : public std::exception {
public:
    Error(const char* message, std::source_location sl = std::source_location::current());

    const char* what() const noexcept override;

private:
    std::string _message;
};

} // namespace
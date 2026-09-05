#pragma once
#include <iostream>
#include <stdexcept>
#include <format>
#include <string>

namespace SockLib
{
    // No need for million gazillion exceptions.
    // I hate when libraries do that.
    // A single exception and a string is enough.

    class Exception : public std::runtime_error
    {
    public:
        template <typename... Args>
        Exception(std::format_string<Args...> fmt, Args&&... args);

        Exception(const std::string &s);
        Exception(const char *s);
    };
}

template <typename... Args>
inline SockLib::Exception::Exception(std::format_string<Args...> fmt, Args&&... args) :
    std::runtime_error(std::format(fmt, std::forward<Args>(args)...))
{
}
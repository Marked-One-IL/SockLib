#ifndef SOCK_LIB_EXCEPTION
#define SOCK_LIB_EXCEPTION
#include <iostream>
#include <stdexcept>
#include <format>
#include <string>

namespace SockLib
{
    // No need for a lot of exceptions types.
    // This is extra work for something is not useful at all and just overwhelms the user.
    
    class Helper;
    class Sock;

    class Exception : public std::runtime_error
    {
    private:
        Exception(const std::string &s);
        Exception(const char *s);

        friend class SockLib::Helper;
        friend class SockLib::Sock;
    };
}
#endif // SOCK_LIB_EXCEPTION
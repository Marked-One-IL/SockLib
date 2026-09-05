#pragma once
#include <SockLib/Helper.hpp>

namespace SockLib
{
    class Sock;

    class Client
    {
    public:
        inline static constexpr const char *LOCALHOST = "127.0.0.1";
        Client(void) = delete;
        static SockLib::Sock connect(const char *address, const char *port);
    };
}
#pragma once
#include <cstdint>
#include <SockLib/Helper.hpp>
#include <SockLib/Sock.hpp>

namespace SockLib
{
    class Server
    {
    public:
        Server(std::uint16_t port, bool localhost);
        SockLib::Sock accept(void);

    private:
        SockLib::Sock m_sock;
    };
}
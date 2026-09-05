#pragma once
#include <cstdint>
#include <SockLib/Sock.hpp>

namespace SockLib
{
    class Server
    {
    public:
        Server(std::uint16_t port, bool localhost);
        SockLib::Sock acceptClient(void);

    private:
        SockLib::Helper::Sock m_sock;
    };
}
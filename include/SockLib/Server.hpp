#pragma once
#include <cstdint>
#include <SockLib/Helper.hpp>
#include <SockLib/Sock.hpp>

namespace SockLib
{
    class Server
    {
    public:
        Server(SockLib::Helper::PortType port, bool localhost);
        SockLib::Sock acceptClient(void);

    private:
        SockLib::Helper::Sock m_sock;
    };
}
#include <SockLib/Server.hpp>

SockLib::Server::Server(SockLib::Helper::PortType port, bool localhost) :
    m_sock(SockLib::Helper::serverInit(port, localhost))
{
}

SockLib::Sock SockLib::Server::acceptClient(void)
{
    return SockLib::Helper::accept(this->m_sock);
}
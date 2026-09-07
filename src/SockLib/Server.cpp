#include <SockLib/Server.hpp>

SockLib::Server::Server(std::uint16_t port, bool localhost) :
    m_sock(SockLib::Helper::serverInit(port, localhost))
{
}

SockLib::Sock SockLib::Server::accept(void)
{
    return SockLib::Helper::accept(this->m_sock);
}
#include <SockLib/Server.hpp>
#include <SockLib/Helper.hpp>

SockLib::Server::Server(std::uint16_t port, bool localhost) :
    m_sock(SockLib::Helper::serverInit(port, localhost))
{
}

SockLib::Sock SockLib::Server::acceptClient(void)
{
    return SockLib::Helper::accept(this->m_sock);
}
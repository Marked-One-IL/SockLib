#include <SockLib/Server.hpp>

SockLib::Server::Server(std::uint16_t port, bool localhost) :
    m_sock(SockLib::Helper::serverInit(static_cast<SockLib::Helper::PortType>(port), localhost))
{
}

SockLib::Sock SockLib::Server::accept(std::size_t timeoutMS)
{
    SockLib::Sock sock = SockLib::Helper::accept(this->m_sock.m_socket);
    SockLib::Helper::setTimeout(sock.m_socket, static_cast<SockLib::Helper::TimeoutType>(timeoutMS));
    return sock;
}
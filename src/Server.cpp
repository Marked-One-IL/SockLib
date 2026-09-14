#include <SockLib/Server.hpp>
#include <cassert>

SockLib::Server::Server(SockLib::Server &&other) noexcept(true) :
    m_sock(std::move(other.m_sock))
{
}
SockLib::Server &SockLib::Server::operator = (SockLib::Server &&other) noexcept(true)
{
    this->m_sock = std::move(other.m_sock);
}

SockLib::Server::Server(std::uint16_t port, SockLib::Server::Visibility visibility) :
    m_sock(SockLib::Helper::serverInit(static_cast<SockLib::Helper::PortType>(port), visibility == SockLib::Server::Visibility::LOCALHOST ? true : false))
{
}

SockLib::Sock SockLib::Server::accept(std::size_t timeoutMS) const
{
    assert(timeoutMS <= SockLib::Sock::TIMEOUT_LIMIT);
    SockLib::Sock sock = SockLib::Helper::accept(this->m_sock.m_socket);
    SockLib::Helper::setTimeout(sock.m_socket, static_cast<SockLib::Helper::TimeoutType>(timeoutMS));
    return sock;
}
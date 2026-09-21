#include <SockLib/Server.hpp>
#include <cassert>

SockLib::Server::Server(SockLib::Server &&other) noexcept :
    m_sock(std::move(other.m_sock))
{
}
SockLib::Server &SockLib::Server::operator = (SockLib::Server &&other) noexcept
{
    this->m_sock = std::move(other.m_sock);
	return *this;
}

SockLib::Server::Server(std::uint16_t port, SockLib::Server::Visibility visibility) :
    m_sock(SockLib::Helper::serverInit(static_cast<SockLib::Helper::PortType>(port), visibility == SockLib::Server::Visibility::LOCALHOST ? true : false))
{
}

SockLib::Sock SockLib::Server::accept(void) const
{
    SockLib::Sock sock = SockLib::Helper::accept(this->m_sock.m_socket);
    return sock;
}
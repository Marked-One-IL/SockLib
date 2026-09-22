#include <SockLib/Server.hpp>
#include <cassert>

SockLib::Server::Server(SockLib::Server &&other) noexcept :
    m_serverSock(std::move(other.m_serverSock))
{
}
SockLib::Server &SockLib::Server::operator = (SockLib::Server &&other) noexcept
{
    this->m_serverSock = std::move(other.m_serverSock);
	return *this;
}

SockLib::Server::Server(std::uint16_t port, SockLib::Server::Accessibility accessibility, SockLib::Server::IPver ipVersion) :
    m_serverSock(SockLib::Server::initServer(port, accessibility, ipVersion))
{
}
SockLib::Sock SockLib::Server::accept(void) const
{
    return SockLib::Sock(SockLib::Helper::accept(this->m_serverSock.m_sock));
}

SockLib::Sock SockLib::Server::initServer(std::uint16_t port, SockLib::Server::Accessibility accessibility, SockLib::Server::IPver ipVersion)
{ assert((accessibility == SockLib::Server::Accessibility::LOOPBACK_ONLY) || (accessibility == SockLib::Server::Accessibility::ACCESSIBLE)); 
  assert((ipVersion == SockLib::Server::IPver::IPV4_ONLY) || (ipVersion == SockLib::Server::IPver::IPV6_ONLY) || (ipVersion == SockLib::Server::IPver::IPV4N6));

    return SockLib::Sock(SockLib::Helper::serverInit(static_cast<SockLib::Helper::PortType>(port),
        accessibility == SockLib::Server::Accessibility::LOOPBACK_ONLY ? true : false,
        ipVersion == SockLib::Server::IPver::IPV4_ONLY || ipVersion == SockLib::Server::IPver::IPV4N6 ? true : false,
        ipVersion == SockLib::Server::IPver::IPV6_ONLY || ipVersion == SockLib::Server::IPver::IPV4N6 ? true : false));
}
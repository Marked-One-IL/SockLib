#ifndef SOCK_LIB_SERVER
#define SOCK_LIB_SERVER
#include <SockLib/Helper.hpp>
#include <SockLib/Sock.hpp>
#include <cstdint>

namespace SockLib
{
    class Server
    {
    public:
        enum class Visibility : std::uint8_t
        {
            LOCALHOST,
            OPEN
        };

        // Move semantics bs for STL support.
        Server(const SockLib::Server &other) = delete;
        SockLib::Server &operator = (const SockLib::Server &other) = delete;
        Server(SockLib::Server &&other) noexcept(true);
        SockLib::Server &operator = (SockLib::Server &&other) noexcept(true);

        Server(std::uint16_t port, SockLib::Server::Visibility visibility);
        SockLib::Sock accept(std::size_t timeoutMS) const;

    private:
        SockLib::Sock m_sock;
    };
}
#endif // SOCK_LIB_SERVER
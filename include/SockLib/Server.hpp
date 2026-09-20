#ifndef SOCK_LIB_SERVER
#define SOCK_LIB_SERVER
#include <SockLib/Helper.hpp>
#include <SockLib/Sock.hpp>
#include <cstdint>
#include <chrono>

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

        // Move semantics for STL support.
        Server(const SockLib::Server &other) = delete;
        SockLib::Server &operator = (const SockLib::Server &other) = delete;
        Server(SockLib::Server &&other) noexcept;
        SockLib::Server &operator = (SockLib::Server &&other) noexcept;

        Server(std::uint16_t port, SockLib::Server::Visibility visibility);

        SockLib::Sock accept(std::chrono::milliseconds timeout = SockLib::Sock::MAX_TIMEOUT) const;

    private:
        SockLib::Sock m_sock;
    };
}
#endif // SOCK_LIB_SERVER
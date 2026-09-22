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
        enum class Reachability : std::uint8_t
        {
            LOOPBACK_ONLY,
            FULLY_REACHABLE
        };
        enum class IPver : std::uint8_t
        {
            IPV4_ONLY,
            IPV6_ONLY,
            BOTH_IPV4N6
        };

        Server(const SockLib::Server &other) = delete;
        SockLib::Server &operator = (const SockLib::Server &other) = delete;
        Server(SockLib::Server &&other) noexcept;
        SockLib::Server &operator = (SockLib::Server &&other) noexcept;

        Server(std::uint16_t port, SockLib::Server::Reachability reachability, SockLib::Server::IPver ipVersion);
        SockLib::Sock accept(void) const;

    private:
        // This is used to safely assert before construction.
        static SockLib::Sock initServer(std::uint16_t port, SockLib::Server::Reachability reachability, SockLib::Server::IPver ipVersion);
        
        SockLib::Sock m_serverSock;
    };
}
#endif // SOCK_LIB_SERVER
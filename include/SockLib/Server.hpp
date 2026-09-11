#ifndef SOCK_LIB_SERVER
#define SOCK_LIB_SERVER
#include <cstdint>
#include <SockLib/Helper.hpp>
#include <SockLib/Sock.hpp>

namespace SockLib
{
    class Server
    {
    public:
        Server(std::uint16_t port, bool localhost);
        SockLib::Sock accept(std::size_t timeoutMS);

    private:
        SockLib::Sock m_sock;
    };
}
#endif // SOCK_LIB_SERVER
#include <SockLib/Client.hpp>
#include <SockLib/Sock.hpp>
#include <SockLib/Helper.hpp>

SockLib::Sock SockLib::Client::connect(const char *address, const char *port)
{
    return SockLib::Sock(SockLib::Helper::connect(address, port));
}
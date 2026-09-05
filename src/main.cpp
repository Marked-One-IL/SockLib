#include <iostream>
#include <SockLib/Sock.hpp>
#include <SockLib/Client.hpp>
#include <SockLib/Server.hpp>
#include <SockLib/Serializer.hpp>
#include <SockLib/Deserializer.hpp>

int main()
{
    SockLib::Sock s = SockLib::Client::connect(SockLib::Client::LOCALHOST, "80");

    
    std::cout << s.recvStr() << '\n';
}
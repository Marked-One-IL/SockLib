#include <iostream>
#include <SockLib/Sock.hpp>
#include <SockLib/Server.hpp>
#include <SockLib/Serializer.hpp>
#include <SockLib/Deserializer.hpp>

// #define SERVER

int main()
{
    try
    {
#ifdef SERVER
        SockLib::Server server (80, true);
        SockLib::Sock sock = server.accept();
        SockLib::Serializer s;
        s.serializeBool(true);
        s.serializeFloat64(25.25);
        s.serializeStr("I can only look for the sky.\nThe earth is full dust.\nThere is nothing.");
        sock.sendSerialized(s);
        std::cout << sock.recvInt16() << '\n';
#else
        SockLib::Sock sock = SockLib::Sock::connect(SockLib::Sock::LOCALHOST, 80);
        SockLib::Deserializer d = sock.recvDeserialized();
        std::cout << d.deserializeBool() << '\n';
        std::cout << d.deserializeFloat64() << '\n';
        std::cout << d.deserializeStrView() << '\n';
        sock.sendInt16(101);
#endif
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << '\n';
    }
}
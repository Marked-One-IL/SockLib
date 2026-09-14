# About
A simple but explicit TCP socket library for C++.<br>
- The library support Windows, Linux and Mac.<br>

# CMake
Before creating target.<br>
- Make sure you have the directory 'third_party' in your project root.
```cmake
add_subdirectory("third_party/SockLib")
```
After creating target.<br>
```cmake
target_link_libraries(TARGET_NAME PRIVATE SockLib)
```

# Example
```cpp
#include <iostream>
#include <SockLib/Sock.hpp>
#include <SockLib/Server.hpp>
#include <SockLib/Serializer.hpp>
#include <SockLib/Deserializer.hpp>
#include <SockLib/Exception.hpp>

// #define SERVER

int main()
{
    try
    {
#ifdef SERVER
        SockLib::Server server (8080, SockLib::Server::Visibility::LOCALHOST);
        SockLib::Sock sock = server.accept(1000);
        SockLib::Serializer s;
        s.serializeBool(true);
        s.serializeFloat64(25.25);
        s.serializeStr("Hello, World!");
        sock.sendSerialized(s);
        std::cout << std::format("Server: {}\n", sock.recvInt16());
#else // CLIENT
        SockLib::Sock sock = SockLib::Sock::connect(SockLib::Sock::LOCALHOST, 8080, 1000);
        SockLib::Deserializer d = sock.recvDeserialized(4096);
        std::cout << std::format("Client: {}\n", d.deserializeBool());
        std::cout << std::format("Client: {}\n", d.deserializeFloat64());
        std::cout << std::format("Client: {}\n", d.deserializeStrView());
        sock.sendInt16(101);
#endif
    }
    catch (const SockLib::Exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
```

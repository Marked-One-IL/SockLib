# Status
Support windows only at the moment.<br>

# CMake
Before creating target.
- Make sure you have the directory 'third_party' in your project root.
```cmake
add_subdirectory("third_party/SockLib")
```
After creating target.<br>
```cmake
target_link_libraries(${PROJECT_NAME} PRIVATE SockLib)
```

# Example
```cpp
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
        s.serializeStr("Hello, World!");
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
```

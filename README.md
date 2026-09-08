# About
A simple but explicit TCP socket library for C++ and Python.<br>
- The C++ library support Windows, Linux and Mac.<br>

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

# Example C++
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
        SockLib::Server server (8080, true);
        SockLib::Sock sock = server.accept();
        SockLib::Serializer s;
        s.serializeBool(true);
        s.serializeFloat64(25.25);
        s.serializeStr("Hello, World!");
        sock.sendSerialized(s);
        std::cout << sock.recvInt16() << '\n';
#else // CLIENT
        SockLib::Sock sock = SockLib::Sock::connect(SockLib::Sock::LOCALHOST, 8080);
        SockLib::Deserializer d = sock.recvDeserialized();
        std::cout << d.deserializeBool() << '\n';
        std::cout << d.deserializeFloat64() << '\n';
        std::cout << d.deserializeStrView() << '\n';
        sock.sendInt16(101);
#endif
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
```

# Example Python
```py
import sock_lib

SERVER = False

def main():
    try:
        if SERVER:
            server = sock_lib.Server(8080, True)
            sock = server.accept()
            s = sock_lib.Serializer()
            s.serialize_bool(True)
            s.serialize_float64(25.25)
            s.serialize_str("Hello, World!")
            sock.send_serialized(s)
            print(sock.recv_int16())
        else: # CLIENT
            sock = sock_lib.Sock.connect(sock_lib.Sock.LOCAL_HOST, 8080)
            d = sock.recv_deserialized()
            print(d.deserialize_bool())
            print(d.deserialize_float64())
            print(d.deserialize_str())
            sock.send_int16(101)
    except Exception as e:
        print(e)

if __name__ == "__main__":
    main()
```

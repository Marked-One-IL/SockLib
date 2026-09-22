# Notice
The library is frequently updated and so it's design.<br>
While it's currently usable I won't called it finished or my final product.<br>
This repository is currently just public development.<br>

# About
A simple, safe and explicit TCP socket library for C++.<br>
- The library support Windows, Linux and Mac.<br>

# Note
This library is meant to be used on both ends and is flavored.<br>
But raw communication methods are still available for more niche cases.<br>

# CMake
Before creating target.<br>
- Make sure the library is in the directory 'third_party'.
```cmake
add_subdirectory("third_party/SockLib")
```
After creating target.<br>
```cmake
target_link_libraries(TARGET_NAME PRIVATE SockLib)
```

# Example
- Shared Object
```cpp
struct Vec2
{
    SockLib::Obj::Float32 x{};
    SockLib::Obj::Float32 y{};

    void print()
    {
        std::cout << "x: " << x << '\n';
        std::cout << "y: " << y << '\n';
    }

private:
    SOCK_LIB_OBJ_DEFINE_PROTOCOL(x, y)
};
```
- Server
```cpp
SockLib::Server server(8080, SockLib::Server::Reachability::LOOPBACK_ONLY, SockLib::Server::IPver::IPV4_ONLY);
SockLib::Sock sock = server.accept();
Vec2 vec2 = { 25.25, 50.5 };
sock.sendObj(vec2);
```
- Client
```cpp
SockLib::Sock sock = SockLib::Sock::connect(SockLib::Sock::LOOPBACK_IPV4_ADDR, 8080);
Vec2 vec2 = sock.recvObjStatic<Vec2>();
vec2.print();
```

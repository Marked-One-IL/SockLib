# About
A simple, safe and explicit TCP socket library for C++.<br>
- The library support Windows, Linux and Mac.<br>

# Note
This library is meant to be used on both ends and is flavored.<br>
But raw communication methods are still available for more niche cases.<br>

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

    SOCK_LIB_OBJ_TRANSMISSION_ORDER(x, y)
};
```
- Server
```cpp
SockLib::Server server(8080, SockLib::Server::Visibility::LOCALHOST);
SockLib::Sock sock = server.accept();
Vec2 vec2 = { 25.25, 50.5 };
sock.sendObj(vec2);
```
- Client
```cpp
SockLib::Sock sock = SockLib::Sock::connect(SockLib::Sock::LOCALHOST, 8080);
Vec2 vec2 = sock.recvObjStatic<Vec2>();
vec2.print();
```
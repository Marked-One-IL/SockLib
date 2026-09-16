#include <SockLib/Server.hpp>
#include <SockLib/Sock.hpp>
#include <SockLib/Obj.hpp>

struct Vec2
{
    SockLib::Obj::Float x;
    SockLib::Obj::Float y;

    SOCK_LIB_OBJ_TRANSMISSION_ORDER(x, y)
};

struct Person
{
    SockLib::Obj::Str name;
    SockLib::Obj::Int age;
    SockLib::Obj::Float height;
    Vec2 pos;

    SOCK_LIB_OBJ_TRANSMISSION_ORDER(name, age, height, pos)
};

int main()
{
    SockLib::Server server(8080, SockLib::Server::Visibility::LOCALHOST);
    auto sock = server.accept(1000);

    Person p ( "Shane Walsh", 37, 1.8, {20.25, 32.43} );
    sock.sendObj(p);

    return 0;
}
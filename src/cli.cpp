#include <iostream>
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
    auto sock = SockLib::Sock::connect(SockLib::Sock::LOCALHOST, 8080, 1000);

    Person me = sock.recvObj<Person>(50);

    std::cout << me.name     << '\n';
    std::cout << me.age      << '\n';
    std::cout << me.height   << '\n';
    std::cout << me.pos.x    << '\n';
    std::cout << me.pos.y    << '\n';

    return 0;
}
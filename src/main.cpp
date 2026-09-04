#include <iostream>
#include <Socket/DataSerializer.hpp>
#include <Socket/DataDeserializer.hpp>

int main()
{
    // Sender
    Socket::DataSerializer p;
    p.serializeInt(10);
    p.serializeBool(true);
    p.serializeStr("1234");
    p.serializeFloat(132.123f);

    // Receiver
    Socket::DataDeserializer e(p.m_bytes.data(), p.m_bytes.size());
    int v1 = e.deserializeInt();
    bool v2 = e.deserializeBool();
    std::string v3 = e.deserializeStr();
    float v4 = e.deserializeFloat();

    std::cout << v1 << '\n';
    std::cout << v2 << '\n';
    std::cout << v3 << '\n';
    std::cout << v4 << '\n';
}
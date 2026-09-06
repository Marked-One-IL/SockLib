#include <SockLib/Sock.hpp>
#include <cstring>
#include <cassert>

// These move semantics are a must if I want classes like std::vector<T> not to close sockets when reallocating.

SockLib::Sock::~Sock(void)
{
    if (SockLib::Helper::INVALID_SOCK != this->m_socket) {
        this->close();
    }
}
SockLib::Sock::Sock(SockLib::Sock &&other) noexcept(true) :
    m_socket(other.m_socket)
{
    other.m_socket = SockLib::Helper::INVALID_SOCK;
}
SockLib::Sock &SockLib::Sock::operator = (SockLib::Sock &&other) noexcept(true)
{
    assert(this != &other);
    this->close();
    this->m_socket = other.m_socket;
    other.m_socket = SockLib::Helper::INVALID_SOCK;
    return *this;
}

void SockLib::Sock::setTimeout(SockLib::Helper::Size ms)
{
    SockLib::Helper::setTimeout(this->m_socket, ms);
}
void SockLib::Sock::close(void)
{
    SockLib::Helper::close(this->m_socket);
    this->m_socket = SockLib::Helper::INVALID_SOCK;
}

void SockLib::Sock::sendSerialized(const SockLib::Serializer &d)
{
    this->sendUint32(static_cast<std::uint32_t>(d.m_bytes.size()));
    this->sendAllBytes(d.getBytes(), d.getSize());
}
SockLib::Deserializer SockLib::Sock::recvDeserialized(void)
{
    std::uint32_t size = this->recvUint32();
    SockLib::Deserializer data(static_cast<SockLib::Helper::Size>(size));
    this->recvAllBytes(data.getBytes(), static_cast<SockLib::Helper::Size>(size));
    return data;
}

void SockLib::Sock::sendInt8(std::int8_t i)
{
    this->sendAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
}
void SockLib::Sock::sendUint8(std::uint8_t i)
{
    this->sendAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
}
void SockLib::Sock::sendInt16(std::int16_t i)
{
    i = static_cast<std::int16_t>(SockLib::Helper::normalizeUint16(static_cast<std::uint16_t>(i)));
    this->sendAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
}
void SockLib::Sock::sendUint16(std::uint16_t i)
{
    i = SockLib::Helper::normalizeUint16(i);
    this->sendAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
}
void SockLib::Sock::sendInt32(std::int32_t i)
{
    i = static_cast<std::int32_t>(SockLib::Helper::normalizeUint32(static_cast<std::uint32_t>(i)));
    this->sendAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
}
void SockLib::Sock::sendUint32(std::uint32_t i)
{
    i = SockLib::Helper::normalizeUint32(i);
    this->sendAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
}
void SockLib::Sock::sendInt64(std::int64_t i)
{
    i = static_cast<std::int64_t>(SockLib::Helper::normalizeUint64(static_cast<std::uint64_t>(i)));
    this->sendAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
}
void SockLib::Sock::sendUint64(std::uint64_t i)
{
    i = SockLib::Helper::normalizeUint64(i);
    this->sendAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
}
void SockLib::Sock::sendFloat32(SockLib::Helper::float32_t f)
{
    std::uint32_t i{};
    std::memcpy(&i, &f, sizeof(i));
    this->sendUint32(i);
}
void SockLib::Sock::sendFloat64(SockLib::Helper::float64_t f)
{
    std::uint64_t i{};
    std::memcpy(&i, &f, sizeof(i));
    this->sendUint64(i);
}
void SockLib::Sock::sendAllBytes(const SockLib::Helper::Byte *bytes, SockLib::Helper::Size size)
{
    SockLib::Helper::sendAll(this->m_socket, bytes, size);
}
SockLib::Helper::Size SockLib::Sock::sendSomeBytes(const SockLib::Helper::Byte *bytes, SockLib::Helper::Size size)
{
    return SockLib::Helper::send(this->m_socket, bytes, size);
}
void SockLib::Sock::sendBool(bool b)
{
    this->sendUint8(static_cast<std::uint8_t>(b));
}
void SockLib::Sock::sendChar(char c)
{
    this->sendInt8(static_cast<std::int8_t>(c));
}
void SockLib::Sock::sendInt(int i)
{
    this->sendInt32(static_cast<std::int32_t>(i));
}
void SockLib::Sock::sendFloat(float f)
{
    this->sendFloat32(static_cast<SockLib::Helper::float32_t>(f));
}
void SockLib::Sock::sendStr(std::string_view s)
{
    this->sendUint32(static_cast<std::uint32_t>(s.size()));
    this->sendAllBytes(reinterpret_cast<const SockLib::Helper::Byte*>(s.data()), static_cast<SockLib::Helper::Size>(s.size()));
}

std::int8_t SockLib::Sock::recvInt8(void)
{
    std::int8_t i{};
    this->recvAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
    return i;
}
std::uint8_t SockLib::Sock::recvUint8(void)
{
    std::uint8_t i{};
    this->recvAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
    return i;
}
std::int16_t SockLib::Sock::recvInt16(void)
{
    std::int16_t i{};
    this->recvAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
    i = static_cast<std::int16_t>(SockLib::Helper::normalizeUint16(static_cast<std::uint16_t>(i)));
    return i;
}
std::uint16_t SockLib::Sock::recvUint16(void)
{
    std::uint16_t i{};
    this->recvAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
    i = SockLib::Helper::normalizeUint16(i);
    return i;
}
std::int32_t SockLib::Sock::recvInt32(void)
{
    std::int32_t i{};
    this->recvAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
    i = static_cast<std::int32_t>(SockLib::Helper::normalizeUint32(static_cast<std::uint32_t>(i)));
    return i;
}
std::uint32_t SockLib::Sock::recvUint32(void)
{
    std::uint32_t i{};
    this->recvAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
    i = SockLib::Helper::normalizeUint32(i);
    return i;
}
std::int64_t SockLib::Sock::recvInt64(void)
{
    std::int64_t i{};
    this->recvAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
    i = static_cast<std::int64_t>(SockLib::Helper::normalizeUint64(static_cast<std::uint64_t>(i)));
    return i;
}
std::uint64_t SockLib::Sock::recvUint64(void)
{
    std::uint64_t i{};
    this->recvAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(&i), static_cast<SockLib::Helper::Size>(sizeof(i)));
    i = SockLib::Helper::normalizeUint64(i);
    return i;
}
SockLib::Helper::float32_t SockLib::Sock::recvFloat32(void)
{
    std::uint32_t i = this->recvUint32();
    SockLib::Helper::float32_t f{};
    std::memcpy(&f, &i, sizeof(f));
    return f;
}
SockLib::Helper::float64_t SockLib::Sock::recvFloat64(void)
{
    std::uint64_t i = this->recvUint64();
    SockLib::Helper::float64_t f{};
    std::memcpy(&f, &i, sizeof(f));
    return f;
}
void SockLib::Sock::recvAllBytes(SockLib::Helper::Byte *bytes, SockLib::Helper::Size size)
{
    SockLib::Helper::recvAll(this->m_socket, bytes, size);
}
SockLib::Helper::Size SockLib::Sock::recvSomeBytes(SockLib::Helper::Byte *bytes, SockLib::Helper::Size size)
{
    return SockLib::Helper::recv(this->m_socket, bytes, size);
}
bool SockLib::Sock::recvBool(void)
{
    return static_cast<bool>(this->recvUint8());
}
char SockLib::Sock::recvChar(void)
{
    return static_cast<char>(this->recvInt8());
}
int SockLib::Sock::recvInt(void)
{
    return static_cast<int>(this->recvInt32());
}
float SockLib::Sock::recvFloat(void)
{
    return static_cast<float>(this->recvFloat32());
}
std::string SockLib::Sock::recvStr(void)
{
    std::uint32_t size = this->recvUint32();
    std::string s(static_cast<std::string::size_type>(size), '\0');
    this->recvAllBytes(reinterpret_cast<SockLib::Helper::Byte*>(s.data()), static_cast<SockLib::Helper::Size>(size));
    return s;
}

SockLib::Sock::Sock(SockLib::Helper::Sock new_socket) :
    m_socket(new_socket)
{
}
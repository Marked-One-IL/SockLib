#include <SockLib/Sock.hpp>
#include <SockLib/Exception.hpp>
#include <cstring>
#include <cassert>
#include <cmath>

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

void SockLib::Sock::close(void)
{
    SockLib::Helper::close(this->m_socket);
    this->m_socket = SockLib::Helper::INVALID_SOCK;
}

void SockLib::Sock::sendSerialized(const SockLib::Serializer &s)
{
    assert(s.getSize() <= SockLib::Sock::SIZE_LIMIT);
    this->sendUint32(static_cast<std::uint32_t>(s.m_bytes.size()));
    this->sendRawAllBytes(s.getBytes(), s.getSize());
}
SockLib::Deserializer SockLib::Sock::recvDeserialized(std::uint32_t limit)
{
    std::uint32_t size = this->recvUint32();

    if (static_cast<std::size_t>(size) > SockLib::Sock::SIZE_LIMIT) {
        this->close();
        throw SockLib::Exception("Received deserialized data size exceeds SockLib::Sock::SIZE_LIMIT");
    }
    if (size > limit) {
        this->close();
        throw SockLib::Exception("Received deserialized data size exceeds given limit");
    }

    SockLib::Deserializer d(static_cast<std::size_t>(size), *this);
    this->recvRawAllBytes(d.getBytes(), static_cast<std::size_t>(size));
    return d;
}

void SockLib::Sock::sendInt8(std::int8_t i)
{
    this->sendRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
}
void SockLib::Sock::sendUint8(std::uint8_t i)
{
    this->sendRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
}
void SockLib::Sock::sendInt16(std::int16_t i)
{
    i = static_cast<std::int16_t>(SockLib::Helper::normalizeUint16(static_cast<std::uint16_t>(i)));
    this->sendRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
}
void SockLib::Sock::sendUint16(std::uint16_t i)
{
    i = SockLib::Helper::normalizeUint16(i);
    this->sendRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
}
void SockLib::Sock::sendInt32(std::int32_t i)
{
    i = static_cast<std::int32_t>(SockLib::Helper::normalizeUint32(static_cast<std::uint32_t>(i)));
    this->sendRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
}
void SockLib::Sock::sendUint32(std::uint32_t i)
{
    i = SockLib::Helper::normalizeUint32(i);
    this->sendRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
}
void SockLib::Sock::sendInt64(std::int64_t i)
{
    i = static_cast<std::int64_t>(SockLib::Helper::normalizeUint64(static_cast<std::uint64_t>(i)));
    this->sendRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
}
void SockLib::Sock::sendUint64(std::uint64_t i)
{
    i = SockLib::Helper::normalizeUint64(i);
    this->sendRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
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
void SockLib::Sock::sendBytes(const std::byte *bytes, std::size_t size)
{
    assert(size <= SockLib::Sock::SIZE_LIMIT);
    this->sendUint32(static_cast<std::size_t>(size));
    this->sendRawAllBytes(bytes, size);
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
    this->sendBytes(reinterpret_cast<const std::byte*>(s.data()), static_cast<std::size_t>(s.size());
}

std::int8_t SockLib::Sock::recvInt8(void)
{
    std::int8_t i{};
    this->recvRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    return i;
}
std::uint8_t SockLib::Sock::recvUint8(void)
{
    std::uint8_t i{};
    this->recvRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    return i;
}
std::int16_t SockLib::Sock::recvInt16(void)
{
    std::int16_t i{};
    this->recvRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = static_cast<std::int16_t>(SockLib::Helper::normalizeUint16(static_cast<std::uint16_t>(i)));
    return i;
}
std::uint16_t SockLib::Sock::recvUint16(void)
{
    std::uint16_t i{};
    this->recvRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = SockLib::Helper::normalizeUint16(i);
    return i;
}
std::int32_t SockLib::Sock::recvInt32(void)
{
    std::int32_t i{};
    this->recvRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = static_cast<std::int32_t>(SockLib::Helper::normalizeUint32(static_cast<std::uint32_t>(i)));
    return i;
}
std::uint32_t SockLib::Sock::recvUint32(void)
{
    std::uint32_t i{};
    this->recvRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = SockLib::Helper::normalizeUint32(i);
    return i;
}
std::int64_t SockLib::Sock::recvInt64(void)
{
    std::int64_t i{};
    this->recvRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = static_cast<std::int64_t>(SockLib::Helper::normalizeUint64(static_cast<std::uint64_t>(i)));
    return i;
}
std::uint64_t SockLib::Sock::recvUint64(void)
{
    std::uint64_t i{};
    this->recvRawAllBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = SockLib::Helper::normalizeUint64(i);
    return i;
}
SockLib::Helper::float32_t SockLib::Sock::recvFloat32(void)
{
    std::uint32_t i = this->recvUint32();
    SockLib::Helper::float32_t f{};
    std::memcpy(&f, &i, sizeof(f));
    if (!std::isfinite(f)) {
        this->close();
        throw SockLib::Exception("Received float32_t is malformed");
    }
    return f;
}
SockLib::Helper::float64_t SockLib::Sock::recvFloat64(void)
{
    std::uint64_t i = this->recvUint64();
    SockLib::Helper::float64_t f{};
    std::memcpy(&f, &i, sizeof(f));
    if (!std::isfinite(f)) {
        this->close();
        throw SockLib::Exception("Received float64_t is malformed");
    }
    return f;
}
std::vector<std::byte> SockLib::Sock::recvBytes(std::uint32_t limit)
{
    std::uint32_t size = this->recvUint32();

    if (static_cast<std::size_t>(size) > SockLib::Sock::SIZE_LIMIT) {
        this->close();
        throw SockLib::Exception("Received bytes size exceeds SockLib::Sock::SIZE_LIMIT");
    }
    if (size > limit) {
        this->close();
        throw SockLib::Exception("Received bytes size exceeds given limit");
    }

    std::vector<std::byte> v(static_cast<std::vector<std::byte>::size_type>(size), std::byte{});
    this->recvRawAllBytes(v.data(), static_cast<std::size_t>(v.size()));
    return v;
}

bool SockLib::Sock::recvBool(void)
{
    std::uint8_t i = this->recvUint8();
    if (i > 1) {
        this->close();
        throw SockLib::Exception("Received bool is malformed");
    }
    return i != 0;
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
std::string SockLib::Sock::recvStr(std::uint32_t limit)
{
    std::uint32_t size = this->recvUint32();
    if (static_cast<std::size_t>(size) > SockLib::Sock::SIZE_LIMIT) {
        this->close();
        throw SockLib::Exception("Received string size exceeds SockLib::Sock::SIZE_LIMIT");
    }
    if (size > limit) {
        this->close();
        throw SockLib::Exception("Received string size exceeds given limit");
    }

    std::string s(static_cast<std::string::size_type>(size), '\0');
    this->recvRawAllBytes(reinterpret_cast<std::byte*>(s.data()), static_cast<std::size_t>(size));

    for (auto c : s) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (uc < '\x20' || uc > '\x7E') {
            this->close();
            throw SockLib::Exception("Received string is malformed");
        }
    }

    return s;
}

void SockLib::Sock::sendRawAllBytes(const std::byte *bytes, std::size_t size)
{
    SockLib::Helper::sendAll(this->m_socket, reinterpret_cast<const SockLib::Helper::Byte*>(bytes), static_cast<SockLib::Helper::Size>(size));
}
void SockLib::Sock::recvRawAllBytes(std::byte *bytes, std::size_t size)
{
    SockLib::Helper::recvAll(this->m_socket, reinterpret_cast<SockLib::Helper::Byte*>(bytes), static_cast<SockLib::Helper::Size>(size));
}

SockLib::Sock SockLib::Sock::connect(const char *address, std::uint16_t port, std::size_t timeoutMS)
{
    std::string portStr = std::to_string(static_cast<int>(port));
    SockLib::Sock sock = SockLib::Sock(SockLib::Helper::connect(address, portStr.c_str()));
    SockLib::Helper::setTimeout(sock.m_socket, static_cast<SockLib::Helper::TimeoutType>(timeoutMS));
    return sock;
}

SockLib::Sock::Sock(SockLib::Helper::Sock new_socket) :
    m_socket(new_socket)
{
}
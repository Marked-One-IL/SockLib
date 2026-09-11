#include <SockLib/Deserializer.hpp>
#include <SockLib/Exception.hpp>
#include <SockLib/Sock.hpp>
#include <cstring>

std::int8_t SockLib::Deserializer::deserializeInt8(void)
{
    std::int8_t i{};
    this->deserializeBytesRaw(reinterpret_cast<std::byte*>(&i), sizeof(i));
    return i;
}
std::uint8_t SockLib::Deserializer::deserializeUint8(void)
{
    std::uint8_t i{};
    this->deserializeBytesRaw(reinterpret_cast<std::byte*>(&i), sizeof(i));
    return i;
}
std::int16_t SockLib::Deserializer::deserializeInt16(void)
{
    std::int16_t i{};
    this->deserializeBytesRaw(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = static_cast<std::int16_t>(SockLib::Helper::normalizeUint16(static_cast<std::uint16_t>(i)));
    return i;
}
std::uint16_t SockLib::Deserializer::deserializeUint16(void)
{
    std::uint16_t i{};
    this->deserializeBytesRaw(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = SockLib::Helper::normalizeUint16(i);
    return i;
}
std::int32_t SockLib::Deserializer::deserializeInt32(void)
{
    std::int32_t i{};
    this->deserializeBytesRaw(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = static_cast<std::int32_t>(SockLib::Helper::normalizeUint32(static_cast<std::uint32_t>(i)));
    return i;
}
std::uint32_t SockLib::Deserializer::deserializeUint32(void)
{
    std::uint32_t i{};
    this->deserializeBytesRaw(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = SockLib::Helper::normalizeUint32(i);
    return i;
}
std::int64_t SockLib::Deserializer::deserializeInt64(void)
{
    std::int64_t i{};
    this->deserializeBytesRaw(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = static_cast<std::int64_t>(SockLib::Helper::normalizeUint64(static_cast<std::uint64_t>(i)));
    return i;
}
std::uint64_t SockLib::Deserializer::deserializeUint64(void)
{
    std::uint64_t i{};
    this->deserializeBytesRaw(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = SockLib::Helper::normalizeUint64(i);
    return i;
}
SockLib::Helper::float32_t SockLib::Deserializer::deserializeFloat32(void)
{
    std::uint32_t i = this->deserializeUint32();
    SockLib::Helper::float32_t f{};
    std::memcpy(&f, &i, sizeof(f));
    if (!std::isfinite(f)) {
        this->m_originSock.close();
        throw SockLib::Exception("Deserialized float32_t is malformed");
    }
    return f;
}
SockLib::Helper::float64_t SockLib::Deserializer::deserializeFloat64(void)
{
    std::uint64_t i = this->deserializeUint64();
    SockLib::Helper::float64_t f{};
    std::memcpy(&f, &i, sizeof(f));
    if (!std::isfinite(f)) {
        this->m_originSock.close();
        throw SockLib::Exception("Deserialized float64_t is malformed");
    }
    return f;
}
std::vector<std::byte> SockLib::Deserializer::deserializeBytes(void)
{
    std::uint32_t size = this->deserializeUint32();
    std::vector<std::byte> v(static_cast<std::vector<std::byte>::size_type>(size), std::byte{});
    this->deserializeBytesRaw(v.data(), static_cast<std::size_t>(v.size()));
    return v;
}

bool SockLib::Deserializer::deserializeBool(void)
{
    std::uint8_t i = this->deserializeUint8();
    if (i > 1) {
        this->m_originSock.close();
        throw SockLib::Exception("Deserialized bool is malformed");
    }
    return i != 0;
}
char SockLib::Deserializer::deserializeChar(void)
{
    return static_cast<char>(this->deserializeInt8());
}
int SockLib::Deserializer::deserializeInt(void)
{
    return static_cast<int>(this->deserializeInt32());
}
float SockLib::Deserializer::deserializeFloat(void)
{
    return static_cast<float>(this->deserializeFloat32());
}
std::string SockLib::Deserializer::deserializeStrCopy(void)
{
    std::uint32_t size = this->deserializeUint32();
    std::string s(static_cast<std::string::size_type>(size), '\0');
    this->deserializeBytesRaw(reinterpret_cast<std::byte*>(s.data()), static_cast<std::size_t>(size));
    for (auto c : s) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (uc < '\x20' || uc > '\x7E') {
            this->m_originSock.close();
            throw SockLib::Exception("Deserialized string is malformed");
        }
    }

    return s;
}
std::string_view SockLib::Deserializer::deserializeStrView(void)
{
    std::uint32_t size = this->deserializeUint32();
    if ((this->m_totalSize < this->m_current) || ((this->m_totalSize -  this->m_current) < static_cast<std::size_t>(size))) {
        this->m_originSock.close();
        throw SockLib::Exception("Attempted to deserialize more data than available");
    }
    std::string_view s(reinterpret_cast<const char*>(&this->m_bytes[this->m_current]), static_cast<std::string_view::size_type>(size));
    for (auto c : s) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (uc < '\x20' || uc > '\x7E') {
            this->m_originSock.close();
            throw SockLib::Exception("Deserialized string is malformed");
        }
    }

    this->m_current += static_cast<std::size_t>(size);
    return s;
}

void SockLib::Deserializer::deserializeBytesRaw(std::byte* bytes, std::size_t size)
{
    if ((this->m_totalSize < this->m_current) || ((this->m_totalSize - this->m_current) < size)) {
        this->m_originSock.close();
        throw SockLib::Exception("Attempted to deserialize more data than available");
    }
    std::memcpy(bytes, &this->m_bytes[this->m_current], size);
    this->m_current += size;
}

SockLib::Deserializer::Deserializer(std::size_t size, SockLib::Sock &originSock) :
    m_bytes(std::make_unique<std::byte[]>(size)), m_totalSize(size), m_originSock(originSock)
{
}
std::byte *SockLib::Deserializer::getBytes(void)
{
    return this->m_bytes.get();
}
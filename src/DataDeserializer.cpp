#include <Socket/DataDeserializer.hpp>
#include <cstring>
#include <cassert>

std::int8_t Socket::DataDeserializer::deserializeInt8(void)
{
    std::int8_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    return i;
}
std::uint8_t Socket::DataDeserializer::deserializeUint8(void)
{
    std::uint8_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    return i;
}
std::int16_t Socket::DataDeserializer::deserializeInt16(void)
{
    std::int16_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = static_cast<std::int16_t>(Socket::Utils::normalizeUint16(static_cast<std::uint16_t>(i)));
    return i;
}
std::uint16_t Socket::DataDeserializer::deserializeUint16(void)
{
    std::uint16_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = Socket::Utils::normalizeUint16(i);
    return i;
}
std::int32_t Socket::DataDeserializer::deserializeInt32(void)
{
    std::int32_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = static_cast<std::int32_t>(Socket::Utils::normalizeUint32(static_cast<std::uint32_t>(i)));
    return i;
}
std::uint32_t Socket::DataDeserializer::deserializeUint32(void)
{
    std::uint32_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = Socket::Utils::normalizeUint32(i);
    return i;
}
std::int64_t Socket::DataDeserializer::deserializeInt64(void)
{
    std::int64_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = static_cast<std::int64_t>(Socket::Utils::normalizeUint64(static_cast<std::uint64_t>(i)));
    return i;
}
std::uint64_t Socket::DataDeserializer::deserializeUint64(void)
{
    std::uint64_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = Socket::Utils::normalizeUint64(i);
    return i;
}
Socket::Utils::float32_t Socket::DataDeserializer::deserializeFloat32(void)
{
    std::uint32_t i = this->deserializeUint32();
    Socket::Utils::float32_t f{};
    std::memcpy(&f, &i, sizeof(f));
    return f;
}
Socket::Utils::float64_t Socket::DataDeserializer::deserializeFloat64(void)
{
    std::uint64_t i = this->deserializeUint64();
    Socket::Utils::float64_t f{};
    std::memcpy(&f, &i, sizeof(f));
    return f;
}
void Socket::DataDeserializer::deserializeBytes(std::byte *bytes, std::size_t size)
{ 
    assert(this->m_totalSize >= this->m_current);
    assert((this->m_totalSize - this->m_current) >= size);
    std::memcpy(bytes, &this->m_bytes[this->m_current], size);
    this->m_current += size;
}

bool Socket::DataDeserializer::deserializeBool(void)
{
    return static_cast<bool>(this->deserializeUint8());
}
char Socket::DataDeserializer::deserializeChar(void)
{
    return static_cast<char>(this->deserializeInt8());
}
int Socket::DataDeserializer::deserializeInt(void)
{
    return static_cast<int>(this->deserializeInt32());
}
float Socket::DataDeserializer::deserializeFloat(void)
{
    return static_cast<float>(this->deserializeFloat32());
}
std::string Socket::DataDeserializer::deserializeStr(void)
{
    const char *cCasted = reinterpret_cast<const char*>(&this->m_bytes[this->m_current]);
    assert(this->m_totalSize >= this->m_current);
    assert((this->m_totalSize - this->m_current) >= std::strlen(cCasted));
    std::string s(cCasted);
    this->m_current += s.size() + 1;
    return s;
}
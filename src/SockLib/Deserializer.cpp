#include <SockLib/Deserializer.hpp>
#include <cstring>
#include <cassert>

std::int8_t SockLib::Deserializer::deserializeInt8(void)
{
    std::int8_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    return i;
}
std::uint8_t SockLib::Deserializer::deserializeUint8(void)
{
    std::uint8_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    return i;
}
std::int16_t SockLib::Deserializer::deserializeInt16(void)
{
    std::int16_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = static_cast<std::int16_t>(SockLib::Helper::normalizeUint16(static_cast<std::uint16_t>(i)));
    return i;
}
std::uint16_t SockLib::Deserializer::deserializeUint16(void)
{
    std::uint16_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = SockLib::Helper::normalizeUint16(i);
    return i;
}
std::int32_t SockLib::Deserializer::deserializeInt32(void)
{
    std::int32_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = static_cast<std::int32_t>(SockLib::Helper::normalizeUint32(static_cast<std::uint32_t>(i)));
    return i;
}
std::uint32_t SockLib::Deserializer::deserializeUint32(void)
{
    std::uint32_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = SockLib::Helper::normalizeUint32(i);
    return i;
}
std::int64_t SockLib::Deserializer::deserializeInt64(void)
{
    std::int64_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = static_cast<std::int64_t>(SockLib::Helper::normalizeUint64(static_cast<std::uint64_t>(i)));
    return i;
}
std::uint64_t SockLib::Deserializer::deserializeUint64(void)
{
    std::uint64_t i{};
    this->deserializeBytes(reinterpret_cast<std::byte*>(&i), sizeof(i));
    i = SockLib::Helper::normalizeUint64(i);
    return i;
}
SockLib::Helper::float32_t SockLib::Deserializer::deserializeFloat32(void)
{
    std::uint32_t i = this->deserializeUint32();
    SockLib::Helper::float32_t f{};
    std::memcpy(&f, &i, sizeof(f));
    return f;
}
SockLib::Helper::float64_t SockLib::Deserializer::deserializeFloat64(void)
{
    std::uint64_t i = this->deserializeUint64();
    SockLib::Helper::float64_t f{};
    std::memcpy(&f, &i, sizeof(f));
    return f;
}
void SockLib::Deserializer::deserializeBytes(std::byte *bytes, std::size_t size)
{ 
    LOGICAL_FAILURE((this->m_totalSize >= this->m_current) &&
                   ((this->m_totalSize -  this->m_current) >= size), "Deserialized more data more than there is");
    std::memcpy(bytes, &this->m_bytes[this->m_current], size);
    this->m_current += size;
}

bool SockLib::Deserializer::deserializeBool(void)
{
    return static_cast<bool>(this->deserializeUint8());
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
    // [std::uint32_t: 4 bytes][char[]: N]

    std::uint32_t size = this->deserializeUint32();
    std::string s(size, '\0');
    assert(this->m_totalSize >= this->m_current);
    assert((this->m_totalSize - this->m_current) >= size);
    this->deserializeBytes(reinterpret_cast<std::byte*>(s.data()), size);
    return s;
}
std::string_view SockLib::Deserializer::deserializeStrView(void)
{
    // [std::uint32_t: 4 bytes][char[]: N]

    std::uint32_t size = this->deserializeUint32();
    LOGICAL_FAILURE((this->m_totalSize >= this->m_current) &&
                   ((this->m_totalSize -  this->m_current) >= size), "Deserialized more data more than there is");

    std::string_view s(reinterpret_cast<const char*>(&this->m_bytes[this->m_current]), size);
    this->m_current += size;
    return s;
}

SockLib::Deserializer::Deserializer(std::size_t size) :
    m_bytes(std::make_unique<std::byte[]>(size)),
#ifndef NDEBUG // Used by LOGICAL_FAILURE only.
    m_totalSize(size)
#endif
{
}
std::byte *SockLib::Deserializer::getBytes(void)
{
    return this->m_bytes.get();
}
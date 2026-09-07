#include <SockLib/Serializer.hpp>
#include <cstring>
#include <bit>

void SockLib::Serializer::reserve(std::size_t size)
{
    this->m_bytes.reserve(size);
}

void SockLib::Serializer::serializeInt8(std::int8_t i)
{
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void SockLib::Serializer::serializeUint8(std::uint8_t i)
{
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void SockLib::Serializer::serializeInt16(std::int16_t i)
{
    i = static_cast<std::int16_t>(SockLib::Helper::normalizeUint16(static_cast<std::uint16_t>(i)));
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void SockLib::Serializer::serializeUint16(std::uint16_t i)
{
    i = SockLib::Helper::normalizeUint16(i);
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void SockLib::Serializer::serializeInt32(std::int32_t i)
{
    i = static_cast<std::int32_t>(SockLib::Helper::normalizeUint32(static_cast<std::uint32_t>(i)));
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void SockLib::Serializer::serializeUint32(std::uint32_t i)
{
    i = SockLib::Helper::normalizeUint32(i);
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void SockLib::Serializer::serializeInt64(std::int64_t i)
{
    i = static_cast<std::int64_t>(SockLib::Helper::normalizeUint64(static_cast<std::uint64_t>(i)));
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void SockLib::Serializer::serializeUint64(std::uint64_t i)
{
    i = SockLib::Helper::normalizeUint64(i);
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void SockLib::Serializer::serializeFloat32(SockLib::Helper::float32_t f)
{
    std::uint32_t i{};
    std::memcpy(&i, &f, sizeof(i));
    i = SockLib::Helper::normalizeUint32(i);
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void SockLib::Serializer::serializeFloat64(SockLib::Helper::float64_t f)
{
    std::uint64_t i{};
    std::memcpy(&i, &f, sizeof(i));
    i = SockLib::Helper::normalizeUint64(i);
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void SockLib::Serializer::serializeBytes(const std::byte *bytes, std::size_t size)
{
    const std::size_t oldSize = static_cast<std::size_t>(this->m_bytes.size());
    this->m_bytes.resize(oldSize + size, std::byte{});
    std::memcpy(&this->m_bytes[oldSize], bytes, size);
}

void SockLib::Serializer::serializeBool(bool b)
{
    this->serializeUint8(static_cast<std::uint8_t>(b));
}
void SockLib::Serializer::serializeChar(char c)
{
    this->serializeInt8(static_cast<std::int8_t>(c));
}
void SockLib::Serializer::serializeInt(int i)
{
    this->serializeInt32(static_cast<std::int32_t>(i));
}
void SockLib::Serializer::serializeFloat(float f)
{
    this->serializeFloat32(static_cast<SockLib::Helper::float32_t>(f));
}
void SockLib::Serializer::serializeStr(std::string_view v)
{
    this->serializeUint32(static_cast<std::uint32_t>(v.size()));
    this->serializeBytes(reinterpret_cast<const std::byte*>(v.data()), static_cast<std::size_t>(v.size()));
}

const std::byte *SockLib::Serializer::getBytes(void) const
{
    return this->m_bytes.data();
}
std::size_t SockLib::Serializer::getSize(void) const
{
    return static_cast<std::size_t>(this->m_bytes.size());
}
#include <Socket/DataSerializer.hpp>
#include <cstring>
#include <bit>

void Socket::DataSerializer::reserve(std::size_t size)
{
    this->m_bytes.reserve(size);
}

void Socket::DataSerializer::serializeInt8(std::int8_t i)
{
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void Socket::DataSerializer::serializeUint8(std::uint8_t i)
{
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void Socket::DataSerializer::serializeInt16(std::int16_t i)
{
    i = static_cast<std::int16_t>(Socket::Utils::normalizeUint16(static_cast<std::uint16_t>(i)));
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void Socket::DataSerializer::serializeUint16(std::uint16_t i)
{
    i = Socket::Utils::normalizeUint16(i);
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void Socket::DataSerializer::serializeInt32(std::int32_t i)
{
    i = static_cast<std::int32_t>(Socket::Utils::normalizeUint32(static_cast<std::uint32_t>(i)));
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void Socket::DataSerializer::serializeUint32(std::uint32_t i)
{
    i = Socket::Utils::normalizeUint32(i);
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void Socket::DataSerializer::serializeInt64(std::int64_t i)
{
    i = static_cast<std::int64_t>(Socket::Utils::normalizeUint64(static_cast<std::uint64_t>(i)));
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void Socket::DataSerializer::serializeUint64(std::uint64_t i)
{
    i = Socket::Utils::normalizeUint64(i);
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void Socket::DataSerializer::serializeFloat32(Socket::Utils::float32_t f)
{
    std::uint32_t i{};
    std::memcpy(&i, &f, sizeof(i));
    i = Socket::Utils::normalizeUint32(i);
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void Socket::DataSerializer::serializeFloat64(Socket::Utils::float64_t f)
{
    std::uint64_t i{};
    std::memcpy(&i, &f, sizeof(i));
    i = Socket::Utils::normalizeUint64(i);
    this->serializeBytes(reinterpret_cast<const std::byte*>(&i), sizeof(i));
}
void Socket::DataSerializer::serializeBytes(const std::byte* bytes, std::size_t size)
{
    const std::size_t oldSize = this->m_bytes.size();
    this->m_bytes.resize(oldSize + size, std::byte{});
    std::memcpy(&this->m_bytes[oldSize], bytes, size);
}

void Socket::DataSerializer::serializeBool(bool b)
{
    this->serializeUint8(static_cast<std::uint8_t>(b));
}
void Socket::DataSerializer::serializeChar(char c)
{
    this->serializeInt8(static_cast<std::int8_t>(c));
}
void Socket::DataSerializer::serializeInt(int i)
{
    this->serializeInt32(static_cast<std::int32_t>(i));
}
void Socket::DataSerializer::serializeFloat(float f)
{
    this->serializeFloat32(static_cast<Socket::Utils::float32_t>(f));
}
void Socket::DataSerializer::serializeStr(std::string_view v)
{
    const std::size_t oldSize = this->m_bytes.size();
    this->m_bytes.resize(oldSize + v.size() + 1, std::byte{});
    std::memcpy(&this->m_bytes[oldSize], v.data(), v.size());
}
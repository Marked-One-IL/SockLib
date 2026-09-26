#include <SockLib/Sock.hpp>
#include <SockLib/Exception.hpp>
#include <fstream>
#include <memory>
#include <cstring>
#include <cassert>
#include <cmath>

SockLib::Sock SockLib::Sock::connect(const char *address, std::uint16_t port)
{
    std::string portStr = std::to_string(static_cast<int>(port));
    SockLib::Sock sock = SockLib::Sock(SockLib::Helper::connect(address, portStr.c_str()));
    return sock;
}
void SockLib::Sock::setTimeout(std::chrono::milliseconds duration)
{ assert(SockLib::Sock::MAX_TIMEOUT >= duration);

    SockLib::Helper::setTimeout(this->m_sock, static_cast<SockLib::Helper::TimeoutType>(duration.count()));
}
void SockLib::Sock::setOffTimeout(void)
{
    SockLib::Helper::setTimeout(this->m_sock, 0);
}
void SockLib::Sock::close(void)
{
    SockLib::Helper::close(this->m_sock);
}
SockLib::Sock::~Sock(void)
{
    this->close();
}

SockLib::Sock::Sock(SockLib::Sock &&other) noexcept :
    m_sock(other.m_sock)
{
    other.m_sock = SockLib::Helper::INVALID_SOCK;
}
SockLib::Sock &SockLib::Sock::operator = (SockLib::Sock &&other) noexcept
{ assert(&other != this);

    this->close();
    this->m_sock = other.m_sock;
    other.m_sock = SockLib::Helper::INVALID_SOCK;
    return *this;
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
{ assert(std::isfinite(f) == true);

    std::uint32_t i{};
    std::memcpy(&i, &f, sizeof(i));
    this->sendUint32(i);
}
void SockLib::Sock::sendFloat64(SockLib::Helper::float64_t f)
{ assert(std::isfinite(f) == true);

    std::uint64_t i{};
    std::memcpy(&i, &f, sizeof(i));
    this->sendUint64(i);
}
void SockLib::Sock::sendBytes(const std::byte *bytes, std::size_t size)
{ assert((nullptr != bytes) || (0 != size)); assert(SockLib::Sock::MAX_SIZE >= size);

    this->sendUint32(static_cast<std::size_t>(size));
    this->sendRawAllBytes(bytes, size);
}
void SockLib::Sock::sendBytes(const std::vector<std::byte> &bytes)
{
    this->sendBytes(bytes.data(), bytes.size());
}
void SockLib::Sock::sendBytes(std::span<const std::byte> bytes)
{
    this->sendBytes(bytes.data(), bytes.size());
}

void SockLib::Sock::sendBool(bool b)
{
    this->sendUint8(static_cast<std::uint8_t>(b));
}
void SockLib::Sock::sendChar(char c)
{
    this->sendUint8(static_cast<std::uint8_t>(c));
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
#ifndef NDEBUG
    for (auto c : s) {
        unsigned char uc = static_cast<unsigned char>(c);
        assert(('\x20' <= uc) && ('\x7E' >= uc));
    }
#endif
    this->sendBytes(reinterpret_cast<const std::byte*>(s.data()), static_cast<std::size_t>(s.size()));
}
void SockLib::Sock::sendFile(const std::filesystem::path &filePath, SockLib::Sock::FileMode fileMode, std::size_t chunkSize)
{ assert(SockLib::Sock::MAX_SIZE >= chunkSize); assert((fileMode == SockLib::Sock::FileMode::TXT) || (fileMode == SockLib::Sock::FileMode::BIN));

    std::ifstream file;
    file.exceptions(std::ofstream::badbit | std::ofstream::failbit);
    std::unique_ptr<std::byte[]> chunk = std::make_unique<std::byte[]>(chunkSize);

    try
    {
        file.open(filePath, (fileMode == SockLib::Sock::FileMode::BIN) ? std::ios::binary : std::ios::openmode(0));

        file.seekg(0, std::ios::end);
        std::uint64_t size = static_cast<std::uint64_t>(file.tellg());
        if (SockLib::Sock::MAX_FILE_SIZE < size) {
            this->close();
            throw SockLib::Exception(std::format("Sent file size='{}' exceeds SockLib::Sock::MAX_FILE_SIZE", size));
        }
        file.seekg(0, std::ios::beg);
        this->sendUint64(size);

        std::uint64_t remaining = size;
        while (remaining != 0) 
        {
            std::uint64_t currentSize = (remaining > static_cast<std::uint64_t>(chunkSize)) ? static_cast<std::uint64_t>(chunkSize) : remaining;

            file.read(reinterpret_cast<char*>(chunk.get()), static_cast<std::streamsize>(currentSize));
            this->sendRawAllBytes(chunk.get(), static_cast<std::size_t>(currentSize));

            remaining -= currentSize;
        }
    }
    catch (const std::ios_base::failure &e)
    {
        this->close();
        throw SockLib::Exception(std::format("Failed to send file named '{}'\n"
                                             "std::ios_base::failure = '{}'", filePath.string(), e.what()));
    }
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
std::vector<std::byte> SockLib::Sock::recvBytes(std::size_t maxBytes)
{ assert(SockLib::Sock::MAX_SIZE >= maxBytes);

    std::size_t size = static_cast<std::size_t>(this->recvUint32());
    if (SockLib::Sock::MAX_SIZE < size) {
        this->close();
        throw SockLib::Exception(std::format("Received bytes size='{}' exceeds SockLib::Sock::MAX_SIZE", size));
    }
    if (maxBytes < size) {
        this->close();
        throw SockLib::Exception(std::format("Received bytes size='{}' exceeds given max-bytes='{}'", size, maxBytes));
    }

    std::vector<std::byte> v(static_cast<std::vector<std::byte>::size_type>(size), std::byte{});
    this->recvRawAllBytes(reinterpret_cast<std::byte*>(v.data()), size);
    return v;
}

bool SockLib::Sock::recvBool(void)
{
    std::uint8_t i = this->recvUint8();
    if (1 < i) {
        this->close();
        throw SockLib::Exception("Received bool is malformed");
    }
    return i != 0;
}
char SockLib::Sock::recvChar(void)
{
    return static_cast<char>(this->recvUint8());
}
int SockLib::Sock::recvInt(void)
{
    return static_cast<int>(this->recvInt32());
}
float SockLib::Sock::recvFloat(void)
{
    return static_cast<float>(this->recvFloat32());
}
std::string SockLib::Sock::recvStr(std::size_t maxBytes)
{ assert(SockLib::Sock::MAX_SIZE >= maxBytes);

    std::size_t size = static_cast<std::size_t>(this->recvUint32());
    if (SockLib::Sock::MAX_SIZE < size) {
        this->close();
        throw SockLib::Exception(std::format("Received string size='{}' exceeds SockLib::Sock::MAX_SIZE", size));
    }
    if (maxBytes < size) {
        this->close();
        throw SockLib::Exception(std::format("Received string size='{}' exceeds given max-bytes='{}'", size, maxBytes));
    }

    std::string s(static_cast<std::string::size_type>(size), '\0');
    this->recvRawAllBytes(reinterpret_cast<std::byte*>(s.data()), size);

    for (auto c : s) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (('\x20' > uc) || ('\x7E' < uc)) {
            this->close();
            throw SockLib::Exception("Received string is malformed");
        }
    }

    return s;
}
void SockLib::Sock::recvFile(const std::filesystem::path &filePath, SockLib::Sock::FileMode fileMode, std::size_t chunkSize, std::uint64_t maxBytes)
{ assert(SockLib::Sock::MAX_FILE_SIZE >= maxBytes); assert(SockLib::Sock::MAX_SIZE >= chunkSize); assert((fileMode == SockLib::Sock::FileMode::TXT) || (fileMode == SockLib::Sock::FileMode::BIN));

    std::ofstream file;
    file.exceptions(std::ofstream::badbit | std::ofstream::failbit);
    std::unique_ptr<std::byte[]> chunk = std::make_unique<std::byte[]>(chunkSize);

    try
    {
        file.open(filePath, ((fileMode == SockLib::Sock::FileMode::BIN) ? std::ios::binary : std::ios::openmode(0)) | std::ios::trunc);

        std::uint64_t size = this->recvUint64();
        if (SockLib::Sock::MAX_FILE_SIZE < size) {
            this->close();
            throw SockLib::Exception(std::format("Received file size='{}' exceeds SockLib::Sock::MAX_FILE_SIZE", size));
        }
        if (maxBytes < size) {
            this->close();
            throw SockLib::Exception(std::format("Received file size='{}' exceeds given max-bytes='{}'", size, maxBytes));
        }

        std::uint64_t remaining = size;
        while (remaining != 0)
        {
            std::uint64_t currentSize = (remaining > static_cast<std::uint64_t>(chunkSize)) ? static_cast<std::uint64_t>(chunkSize) : remaining;

            this->recvRawAllBytes(chunk.get(), std::size_t(currentSize));
            file.write(reinterpret_cast<const char*>(chunk.get()), static_cast<std::streamsize>(currentSize));

            remaining -= currentSize;
        }
    }
    catch (const std::ios_base::failure &e)
    {
        this->close();
        throw SockLib::Exception(std::format("Failed to receive file named '{}'\n"
                                             "std::ios_base::failure = '{}'", filePath.string(), e.what()));
    }
}

void SockLib::Sock::sendRawAllBytes(const std::byte *bytes, std::size_t size)
{ assert((nullptr != bytes) || (0 != size)); assert(SockLib::Sock::MAX_SIZE >= size);

    SockLib::Helper::sendAll(this->m_sock, reinterpret_cast<const SockLib::Helper::Byte*>(bytes), static_cast<SockLib::Helper::Size>(size));
}
std::size_t SockLib::Sock::sendRawSomeBytes(const std::byte *bytes, std::size_t size)
{ assert((nullptr != bytes) || (0 != size)); assert(SockLib::Sock::MAX_SIZE >= size);

    return static_cast<std::size_t>(SockLib::Helper::send(this->m_sock, reinterpret_cast<const SockLib::Helper::Byte*>(bytes),
           static_cast<SockLib::Helper::Size>(size)));
}
void SockLib::Sock::recvRawAllBytes(std::byte *bytes, std::size_t size)
{ assert((nullptr != bytes) || (0 != size)); assert(SockLib::Sock::MAX_SIZE >= size);

    SockLib::Helper::recvAll(this->m_sock, reinterpret_cast<SockLib::Helper::Byte*>(bytes), static_cast<SockLib::Helper::Size>(size));
}
std::size_t SockLib::Sock::recvRawSomeBytes(std::byte *bytes, std::size_t size)
{ assert((nullptr != bytes) || (0 != size)); assert(SockLib::Sock::MAX_SIZE >= size);
    
    return static_cast<std::size_t>(SockLib::Helper::recv(this->m_sock, reinterpret_cast<SockLib::Helper::Byte*>(bytes),
           static_cast<SockLib::Helper::Size>(size)));
}

SockLib::Sock::Sock(SockLib::Helper::Sock new_socket) :
    m_sock(new_socket)
{
}
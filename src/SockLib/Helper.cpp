#include <SockLib/Helper.hpp>
#include <SockLib/Exception.hpp>
#include <bit>

SockLib::Helper::StaticSocketInitAndDestroyer SockLib::Helper::g_staticSocketInitAndDestroyer;

// All integers must be little endian unlike other implementations (eg: htonl()).
// Most machines today are like that by default.
// Then why not flip the logic and save a couple of instructions?

std::uint16_t SockLib::Helper::normalizeUint16(std::uint16_t v)
{
    if constexpr (std::endian::native == std::endian::big) {
        return SockLib::Helper::uint16BitsSwap(v);
    }
    return v;
}
std::uint32_t SockLib::Helper::normalizeUint32(std::uint32_t v)
{
    if constexpr (std::endian::native == std::endian::big) {
        return SockLib::Helper::uint32BitsSwap(v);
    }
    return v;
}
std::uint64_t SockLib::Helper::normalizeUint64(std::uint64_t v)
{
    if constexpr (std::endian::native == std::endian::big) {
        return SockLib::Helper::uint64BitsSwap(v);
    }
    return v;
}

std::uint16_t SockLib::Helper::uint16BitsSwap(std::uint16_t v)
{
    return (v >> 8) | (v << 8);
}
std::uint32_t SockLib::Helper::uint32BitsSwap(std::uint32_t v)
{
    return ((v & 0x000000FFu) << 24) | 
            ((v & 0x0000FF00u) << 8)  |
            ((v & 0x00FF0000u) >> 8)  |
            ((v & 0xFF000000u) >> 24);
}
std::uint64_t SockLib::Helper::uint64BitsSwap(std::uint64_t v)
{
    return ((v & 0x00000000000000FFULL) << 56) |
            ((v & 0x000000000000FF00ULL) << 40) |
            ((v & 0x0000000000FF0000ULL) << 24) |
            ((v & 0x00000000FF000000ULL) << 8)  |
            ((v & 0x000000FF00000000ULL) >> 8)  |
            ((v & 0x0000FF0000000000ULL) >> 24) |
            ((v & 0x00FF000000000000ULL) >> 40) |
            ((v & 0xFF00000000000000ULL) >> 56);
}

SockLib::Helper::Sock SockLib::Helper::serverInit(std::uint16_t port, bool localhost)
{
#ifdef _WIN32
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock) {
        throw SockLib::Exception("Failed to initialize socket (WSA error {})", WSAGetLastError());
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(localhost ? INADDR_LOOPBACK : INADDR_ANY);
    serverAddress.sin_port = htons(static_cast<u_short>(port));

    if (SOCKET_ERROR == bind(sock, (sockaddr*)&serverAddress, sizeof(serverAddress))) {
        int err = WSAGetLastError();
        closesocket(sock);
        throw SockLib::Exception("Failed to bind server on port '{}' (WSA error {})", port, err);
    }
    if (SOCKET_ERROR == listen(sock, SOMAXCONN)) {
        int err = WSAGetLastError();
        closesocket(sock);
        throw SockLib::Exception("Failed to make server listen on port '{}' (WSA error {})", port, err);
    }

    return sock;
#else
    // Put your unsupported platform specific code here.
#endif
}
SockLib::Helper::Sock SockLib::Helper::connect(const char *address, const char *port)
{
#ifdef _WIN32
    addrinfo hints{};
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    addrinfo* result = nullptr;
    int err = getaddrinfo(address, port, &hints, &result);
    if (0 != err) { 
        throw SockLib::Exception("Could not resolve '{}:{}' (getaddrinfo error {})", address, port, err);
    }

    err = 0;
    for (addrinfo* current = result; current != nullptr; current = current->ai_next)
    {
        SOCKET sock = socket(current->ai_family, current->ai_socktype, current->ai_protocol);

        if (INVALID_SOCKET == sock) {
            err = WSAGetLastError();
            continue;
        }
        if (0 == ::connect(sock, current->ai_addr, static_cast<int>(current->ai_addrlen)))
        {
            freeaddrinfo(result);
            return sock;
        }

        err = WSAGetLastError();
        closesocket(sock);
    }
    freeaddrinfo(result);

    throw SockLib::Exception("Could not connect to '{}:{}' (WSA error {})", address, port, err);
#else
    // Put your unsupported platform specific code here.
#endif
}
SockLib::Helper::Sock SockLib::Helper::accept(SockLib::Helper::Sock sock)
{
#ifdef _WIN32
    SOCKET newSock = ::accept(sock, nullptr, nullptr);
    if (INVALID_SOCKET == newSock) {
        throw SockLib::Exception("Failed to accept client (WSA error {})", WSAGetLastError());
    }
    return newSock;
#else
    // Put your unsupported platform specific code here.
#endif
}
std::size_t SockLib::Helper::send(SockLib::Helper::Sock sock, const std::byte *bytes, std::size_t size)
{
#ifdef _WIN32
    int res = ::send(sock, reinterpret_cast<const char*>(bytes), static_cast<int>(size), 0);
    if (SOCKET_ERROR == res || ((0 != size) && (0 == res))) {
        throw SockLib::Exception("Failed to send data (WSA error {})", WSAGetLastError());
    }
    return static_cast<std::size_t>(res);
#else
    // Put your unsupported platform specific code here.
#endif
}
void SockLib::Helper::sendAll(SockLib::Helper::Sock sock, const std::byte *bytes, std::size_t size)
{
    std::size_t sent = 0;
    while (sent < size)
    {
        sent += SockLib::Helper::send(sock, bytes + sent, size - sent);
    }
}
std::size_t SockLib::Helper::recv(SockLib::Helper::Sock sock, std::byte *bytes, std::size_t size)
{
#ifdef _WIN32
    int res = ::recv(sock, reinterpret_cast<char*>(bytes), static_cast<int>(size), 0);
    if (SOCKET_ERROR == res || ((0 != size) && (0 == res))) {
        throw SockLib::Exception("Failed to receive data (WSA error {})", WSAGetLastError());
    }
    return static_cast<std::size_t>(res);
#else
    // Put your unsupported platform specific code here.
#endif
}
void SockLib::Helper::recvAll(SockLib::Helper::Sock sock, std::byte *bytes, std::size_t size)
{
    std::size_t received = 0;
    while (received < size)
    {
        received += SockLib::Helper::recv(sock, bytes + received, size - received);
    }
}
void SockLib::Helper::setTimeout(SockLib::Helper::Sock sock, std::size_t ms)
{
#ifdef _WIN32
    int castedMs = static_cast<int>(ms);
    if (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&castedMs), sizeof(castedMs))) {
        throw SockLib::Exception("Failed to set receive timeout (WSA error {})", WSAGetLastError());
    }
    if (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&castedMs), sizeof(castedMs))) {
        throw SockLib::Exception("Failed to set send timeout (WSA error {})", WSAGetLastError());
    }
#else
    // Put your unsupported platform specific code here.
#endif
}
void SockLib::Helper::close(SockLib::Helper::Sock sock)
{
#ifdef _WIN32
    closesocket(sock);
#else
    // Put your unsupported platform specific code here.
#endif
}

SockLib::Helper::StaticSocketInitAndDestroyer::StaticSocketInitAndDestroyer(void)
{
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#else
    // Put your unsupported platform specific code here.
#endif
}
SockLib::Helper::StaticSocketInitAndDestroyer::~StaticSocketInitAndDestroyer(void)
{
#ifdef _WIN32
    WSACleanup();
#else
    // Put your unsupported platform specific code here.
#endif
}
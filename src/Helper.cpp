#include <SockLib/Helper.hpp>
#include <SockLib/Exception.hpp>
#include <bit>

#ifdef _WIN32
#include <ws2tcpip.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#else
// Put your unsupported platform specific code here.
#endif

SockLib::Helper::StaticSocketInitAndDestroyer SockLib::Helper::g_staticSocketInitAndDestroyer;

// All integers must be little endian unlike other implementations (eg: htonl()).
// Most machines today are like that by default.
// Then why not flip the logic and save a couple of instructions?

std::uint16_t SockLib::Helper::normalizeUint16(std::uint16_t i)
{
    if constexpr (std::endian::native == std::endian::big) {
        return SockLib::Helper::uint16BitsSwap(i);
    }
    return i;
}
std::uint32_t SockLib::Helper::normalizeUint32(std::uint32_t i)
{
    if constexpr (std::endian::native == std::endian::big) {
        return SockLib::Helper::uint32BitsSwap(i);
    }
    return i;
}
std::uint64_t SockLib::Helper::normalizeUint64(std::uint64_t i)
{
    if constexpr (std::endian::native == std::endian::big) {
        return SockLib::Helper::uint64BitsSwap(i);
    }
    return i;
}

std::uint16_t SockLib::Helper::uint16BitsSwap(std::uint16_t i)
{
    return (i >> 8) | (i << 8);
}
std::uint32_t SockLib::Helper::uint32BitsSwap(std::uint32_t i)
{
    return ((i & 0x000000FFu) << 24) | 
           ((i & 0x0000FF00u) << 8)  |
           ((i & 0x00FF0000u) >> 8)  |
           ((i & 0xFF000000u) >> 24);
}
std::uint64_t SockLib::Helper::uint64BitsSwap(std::uint64_t i)
{
    return ((i & 0x00000000000000FFULL) << 56) |
           ((i & 0x000000000000FF00ULL) << 40) |
           ((i & 0x0000000000FF0000ULL) << 24) |
           ((i & 0x00000000FF000000ULL) << 8)  |
           ((i & 0x000000FF00000000ULL) >> 8)  |
           ((i & 0x0000FF0000000000ULL) >> 24) |
           ((i & 0x00FF000000000000ULL) >> 40) |
           ((i & 0xFF00000000000000ULL) >> 56);
}

#ifdef _WIN32
SockLib::Helper::Sock SockLib::Helper::serverInit(u_short port, bool localhost)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sock) {
        throw SockLib::Exception("Failed to initialize socket (WSA error {})", WSAGetLastError());
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(localhost ? INADDR_LOOPBACK : INADDR_ANY);
    serverAddress.sin_port = htons(port);

    if (SOCKET_ERROR == bind(sock, (sockaddr*)&serverAddress, static_cast<int>(sizeof(serverAddress)))) {
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
}
SockLib::Helper::Sock SockLib::Helper::connect(const char *address, const char *port)
{
    addrinfo hints{};
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    addrinfo *result = nullptr;
    int err = getaddrinfo(address, port, &hints, &result);
    if (0 != err) { 
        throw SockLib::Exception("Could not resolve '{}:{}' (getaddrinfo error {})", address, port, err);
    }

    err = 0;
    for (addrinfo *current = result; current != nullptr; current = current->ai_next)
    {
        SOCKET sock = socket(current->ai_family, current->ai_socktype, current->ai_protocol);

        if (INVALID_SOCKET == sock) {
            err = WSAGetLastError();
            continue;
        }
        if (0 == ::connect(sock, current->ai_addr, static_cast<int>(current->ai_addrlen))) {
            freeaddrinfo(result);
            return sock;
        }

        err = WSAGetLastError();
        closesocket(sock);
    }
    freeaddrinfo(result);

    throw SockLib::Exception("Could not connect to '{}:{}' (WSA error {})", address, port, err);
}
SOCKET SockLib::Helper::accept(SOCKET sock)
{
    SOCKET newSock = ::accept(sock, nullptr, nullptr);
    if (INVALID_SOCKET == newSock) {
        throw SockLib::Exception("Failed to accept client (WSA error {})", WSAGetLastError());
    }
    return newSock;
}
int SockLib::Helper::send(SOCKET sock, const char *bytes, int size)
{
    int sent = ::send(sock, bytes, size, 0);
    if (SOCKET_ERROR == sent) {
        throw SockLib::Exception("Failed to send data (WSA error {})", WSAGetLastError());
    }
    return sent;
}
void SockLib::Helper::sendAll(SOCKET sock, const char *bytes, int size)
{
    int sent = 0;
    while (sent < size) {
        sent += SockLib::Helper::send(sock, bytes + sent, size - sent);
    }
}
int SockLib::Helper::recv(SOCKET sock, char *bytes, int size)
{
    int received = ::recv(sock, bytes, size, 0);
    if (SOCKET_ERROR == received) {
        throw SockLib::Exception("Failed to receive data (WSA error {})", WSAGetLastError());
    }
    if (0 == received) {
        throw SockLib::Exception("Failed to receive data because the session ended");
    }
    return received;
}
void SockLib::Helper::recvAll(SOCKET sock, char *bytes, int size)
{
    int received = 0;
    while (received < size) {
        received += SockLib::Helper::recv(sock, bytes + received, size - received);
    }
}
void SockLib::Helper::setTimeout(SOCKET sock, int ms)
{
    if (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&ms), static_cast<int>(sizeof(ms)))) {
        throw SockLib::Exception("Failed to set receive timeout (WSA error {})", WSAGetLastError());
    }
    if (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&ms), static_cast<int>(sizeof(ms)))) {
        throw SockLib::Exception("Failed to set send timeout (WSA error {})", WSAGetLastError());
    }
}
void SockLib::Helper::close(SOCKET sock)
{
    closesocket(sock);
}

#elif defined(__linux__) || defined(__APPLE__)
int SockLib::Helper::serverInit(std::uint16_t port, bool localhost)
{
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == sock) {
        throw SockLib::Exception("Failed to initialize socket (errno error {})", errno);
    }
#ifdef __APPLE__
    int err = SockLib::Helper::disableSigpipe(sock);
    if (err) {
        ::close(sock);
        throw SockLib::Exception("Failed to disable SIGPIPE (errno error {})", err);
    }
#endif

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(localhost ? INADDR_LOOPBACK : INADDR_ANY);
    serverAddress.sin_port = htons(port);

    if (-1 == bind(sock, (sockaddr*)&serverAddress, static_cast<socklen_t>(sizeof(serverAddress)))) {
        int err = errno;
        ::close(sock);
        throw SockLib::Exception("Failed to bind server on port '{}' (errno error {})", port, err);
    }
    if (-1 == listen(sock, SOMAXCONN)) {
        int err = errno;
        ::close(sock);
        throw SockLib::Exception("Failed to make server listen on port '{}' (errno error {})", port, err);
    }

    return sock;
}
int SockLib::Helper::connect(const char *address, const char *port)
{
    addrinfo hints{};
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    addrinfo *result = nullptr;
    int err = getaddrinfo(address, port, &hints, &result);
    if (0 != err) { 
        throw SockLib::Exception("Could not resolve '{}:{}' (getaddrinfo error {})", address, port, err);
    }

    err = 0;
    for (addrinfo *current = result; current != nullptr; current = current->ai_next)
    {
        restart:
        int sock = socket(current->ai_family, current->ai_socktype, current->ai_protocol);
        if (-1 == sock) {
            err = errno;
            continue;
        }
#ifdef __APPLE__
        int err2 = SockLib::Helper::disableSigpipe(sock);
        if (err2) {
            ::close(sock);
            freeaddrinfo(result);
            throw SockLib::Exception("Failed to disable SIGPIPE (errno error {})", err2);
        }
#endif

        if (0 == ::connect(sock, current->ai_addr, static_cast<socklen_t>(current->ai_addrlen))) {
            freeaddrinfo(result);
            return sock;
        }

        err = errno;
        ::close(sock);
        if (EINTR == err) {
            goto restart;
        }
    }
    freeaddrinfo(result);
    throw SockLib::Exception("Could not connect to '{}:{}' (errno error {})", address, port, err);
}
int SockLib::Helper::accept(int sock)
{
    restart:
    int newSock = ::accept(sock, nullptr, nullptr);
    if (-1 == newSock) {
        if (EINTR == errno) {
            goto restart;
        }
        throw SockLib::Exception("Failed to accept client (errno error {})", errno);
    }
#ifdef __APPLE__
    int err = SockLib::Helper::disableSigpipe(newSock);
    if (err) {
        ::close(newSock);
        throw SockLib::Exception("Failed to disable SIGPIPE (errno error {})", err);
    }
#endif
    return newSock;
}
ssize_t SockLib::Helper::send(int sock, const void *bytes, std::size_t size)
{
    restart:
    ssize_t sent = ::send(sock, bytes, size, SockLib::Helper::sendFlag);
    if (-1 == sent) {
        if (EINTR == errno) {
            goto restart;
        }
        throw SockLib::Exception("Failed to send data (errno error {})", errno);
    }
    return sent;
}
void SockLib::Helper::sendAll(int sock, const void *bytes, std::size_t size)
{
    std::size_t sent = 0;
    while (sent < size) {
        const std::byte *currentBytes = reinterpret_cast<const std::byte*>(bytes) + sent;
        sent += static_cast<std::size_t>(SockLib::Helper::send(sock, currentBytes, size - sent));
    }
}
ssize_t SockLib::Helper::recv(int sock, void *bytes, std::size_t size)
{    
    restart:
    ssize_t received = ::recv(sock, bytes, size, 0);
    if (-1 == received) {
        if (EINTR == errno) {
            goto restart;
        }
        throw SockLib::Exception("Failed to receive data (errno error {})", errno);
    }
    if (0 == received) {
        throw SockLib::Exception("Failed to receive data because the session ended");
    }
    return received;
}
void SockLib::Helper::recvAll(int sock, void *bytes, std::size_t size)
{
    std::size_t received = 0;
    while (received < size) {
        std::byte *currentBytes = reinterpret_cast<std::byte*>(bytes) + received;
        received += static_cast<std::size_t>(SockLib::Helper::recv(sock, currentBytes, size - received));
    }
}
void SockLib::Helper::setTimeout(int sock, int ms)
{
    struct timeval tv;
    tv.tv_sec  = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;

    if (-1 == setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, static_cast<socklen_t>(sizeof(tv)))) {
        throw SockLib::Exception("Failed to set receive timeout (errno error {})", errno);
    }
    if (-1 == setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, static_cast<socklen_t>(sizeof(tv)))) {
        throw SockLib::Exception("Failed to set send timeout (errno error {})", errno);
    }
}
void SockLib::Helper::close(int sock)
{
    ::close(sock);
}
#ifdef __APPLE__
int SockLib::Helper::disableSigpipe(int sock)
{
    int value = 1;
    if (-1 == setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &value, static_cast<socklen_t>(sizeof(value)))) {
        return errno;
    }
    return 0;
}
#endif // __APPLE__
#else
// Put your unsupported platform specific code here.
#endif

SockLib::Helper::StaticSocketInitAndDestroyer::StaticSocketInitAndDestroyer(void)
{
#ifdef _WIN32
    WSADATA wsa;
    int res = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (0 != res) {
        throw SockLib::Exception("Failed to initialize WSA (WSAStartup() error {})", res);
    }
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
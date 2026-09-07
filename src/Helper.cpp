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
}
SOCKET SockLib::Helper::accept(SOCKET sock)
{
    SOCKET newSock = ::accept(sock, nullptr, nullptr);
    if (INVALID_SOCKET == newSock) {
        throw SockLib::Exception("Failed to accept std::byte (WSA error {})", WSAGetLastError());
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
    while (sent < size)
    {
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
    while (received < size)
    {
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
#else
// Put your unsupported platform specific code here.
#endif
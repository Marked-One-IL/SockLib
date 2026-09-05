#pragma once
#include <iostream>
#include <format>
#include <limits>
#include <cstdint>
#include <cstddef>
#include <optional>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
// Put your unsupported platform specific code here.
#endif

// Me being overwhelmingly paranoid.
// Almost 100% of all machines support IEEE-754.
// But the C standard does not guarantee it.
static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);
static_assert(std::numeric_limits<float>::is_iec559);
static_assert(std::numeric_limits<double>::is_iec559);

#ifndef NDEBUG
#define LOGICAL_FAILURE(expression, ...) \
do { \
    if (!(expression)) { \
        std::cerr << "\nExpression: " << #expression << '\n'; \
        std::cerr << "Message: " << std::format(__VA_ARGS__) << '\n'; \
        assert(false); \
    } \
} while (false)
#else
#define LOGICAL_FAILURE(expression, ...) \
do { } while (false)
#endif

namespace SockLib
{
    class Sock;
    class Client;
    class Server;

    class Helper
    {
    public:
        using float32_t = float;
        using float64_t = double;
#ifdef _WIN32
        using Sock = SOCKET;
        inline static constexpr auto INVALID_SOCK = INVALID_SOCKET;
#else
        // Put your unsupported platform specific code here.
#endif

        static std::uint16_t normalizeUint16 (std::uint16_t v);
        static std::uint32_t normalizeUint32 (std::uint32_t v);
        static std::uint64_t normalizeUint64 (std::uint64_t v);

        static std::uint16_t uint16BitsSwap  (std::uint16_t v);
        static std::uint32_t uint32BitsSwap  (std::uint32_t v);
        static std::uint64_t uint64BitsSwap  (std::uint64_t v);

    private:
        static SockLib::Helper::Sock serverInit (std::uint16_t port, bool localhost);
        static SockLib::Helper::Sock connect    (const char *address, const char *port);
        static SockLib::Helper::Sock accept     (SockLib::Helper::Sock sock);
        static std::size_t           send       (SockLib::Helper::Sock sock, const std::byte *bytes  , std::size_t size);
        static void                  sendAll    (SockLib::Helper::Sock sock, const std::byte *bytes  , std::size_t size);
        static std::size_t           recv       (SockLib::Helper::Sock sock, std::byte       *bytes  , std::size_t size);
        static void                  recvAll    (SockLib::Helper::Sock sock, std::byte       *bytes  , std::size_t size);
        static void                  setTimeout (SockLib::Helper::Sock sock, std::size_t ms);
        static void                  close      (SockLib::Helper::Sock sock);

        struct StaticSocketInitAndDestroyer
        {
            StaticSocketInitAndDestroyer(void);
            ~StaticSocketInitAndDestroyer(void);
        };

        static SockLib::Helper::StaticSocketInitAndDestroyer g_staticSocketInitAndDestroyer;

        friend class SockLib::Sock;
        friend class SockLib::Client;
        friend class SockLib::Server;
    };
}
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

namespace SockLib
{
    class Sock;
    class Server;

    class Helper
    {
    public:
        using float32_t = float;
        using float64_t = double;
#ifdef _WIN32
        using Size = int;
        using Byte = char;
        using PortType = u_short;
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
#ifdef _WIN32
        using Sock = SOCKET;
        inline static constexpr auto INVALID_SOCK = INVALID_SOCKET;

        static SOCKET serverInit (u_short port, bool localhost);
        static SOCKET connect    (const char *address, const char *port);
        static SOCKET accept     (SOCKET sock);
        static int    send       (SOCKET sock, const char *bytes, int size);
        static void   sendAll    (SOCKET sock, const char *bytes, int size);
        static int    recv       (SOCKET sock, char       *bytes, int size);
        static void   recvAll    (SOCKET sock, char       *bytes, int size);
        static void   setTimeout (SOCKET sock, int ms);
        static void   close      (SOCKET sock);
#else
        // Put your unsupported platform specific code here.
#endif

        struct StaticSocketInitAndDestroyer
        {
            StaticSocketInitAndDestroyer(void);
            ~StaticSocketInitAndDestroyer(void);
        };

        static SockLib::Helper::StaticSocketInitAndDestroyer g_staticSocketInitAndDestroyer;

        friend class SockLib::Sock;
        friend class SockLib::Server;
    };
}
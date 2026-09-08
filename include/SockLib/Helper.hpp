#ifndef SOCK_LIB_HELPER
#define SOCK_LIB_HELPER
#include <cstdint>
#include <cstddef>
#include <limits>

#ifdef _WIN32
#include <winsock2.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <sys/socket.h>
#else
// Put your unsupported platform specific code here.
#endif

// Me being overwhelmingly paranoid.
// Almost 100% of all machines support IEEE-754.
// But the C standard does not guarantee it.
static_assert(sizeof(float)  == 4);
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

        static std::uint16_t normalizeUint16 (std::uint16_t i);
        static std::uint32_t normalizeUint32 (std::uint32_t i);
        static std::uint64_t normalizeUint64 (std::uint64_t i);

        static std::uint16_t uint16BitsSwap  (std::uint16_t i);
        static std::uint32_t uint32BitsSwap  (std::uint32_t i);
        static std::uint64_t uint64BitsSwap  (std::uint64_t i);

    private:
        using TimeoutType = int;
#ifdef _WIN32
        using Size = int;
        using Byte = char;
        using PortType = u_short;
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
#elif defined(__linux__) || defined(__APPLE__)
        using Size = std::size_t;
        using Byte = void;
        using PortType = std::uint16_t;
        using Sock = int;

#ifdef __linux__
// Using ::send() with a broken socket can terminate the program. So we pass a 'MSG_NOSIGNAL' flag and it returns -1 instead.
        inline static constexpr int sendFlag = MSG_NOSIGNAL;
#else // __APPLE__
        inline static constexpr int sendFlag = 0;
        static int disableSigpipe(int sock);
#endif
        inline static constexpr auto INVALID_SOCK = -1;

        static int     serverInit (std::uint16_t port, bool localhost);
        static int     connect    (const char *address, const char *port);
        static int     accept     (int sock);
        static ssize_t send       (int sock, const void *bytes, std::size_t size);
        static void    sendAll    (int sock, const void *bytes, std::size_t size);
        static ssize_t recv       (int sock, void       *bytes, std::size_t size);
        static void    recvAll    (int sock, void       *bytes, std::size_t size);
        static void    setTimeout (int sock, int ms);
        static void    close      (int sock);
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
#endif // SOCK_LIB_HELPER
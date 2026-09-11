#ifndef SOCK_LIB_SOCK
#define SOCK_LIB_SOCK
#include <SockLib/Helper.hpp>
#include <SockLib/Serializer.hpp>
#include <SockLib/Deserializer.hpp>
#include <vector>

namespace SockLib
{
    class Server;

    class Sock
    {
    public:
        inline static constexpr std::size_t SIZE_LIMIT = 2147483647;
        inline static constexpr const char *LOCALHOST = "127.0.0.1";

        ~Sock(void);

        Sock(const SockLib::Sock &other) = delete;
        SockLib::Sock& operator = (const SockLib::Sock &other) = delete;

        Sock(SockLib::Sock &&other) noexcept(true);
        SockLib::Sock& operator = (SockLib::Sock &&other) noexcept(true);

        void close(void);

        void                  sendSerialized   (const SockLib::Serializer &s);
        SockLib::Deserializer recvDeserialized (std::uint32_t limit);

        void sendInt8    (std::int8_t                            i);
        void sendUint8   (std::uint8_t                           i);
        void sendInt16   (std::int16_t                           i);
        void sendUint16  (std::uint16_t                          i);
        void sendInt32   (std::int32_t                           i);
        void sendUint32  (std::uint32_t                          i);
        void sendInt64   (std::int64_t                           i);
        void sendUint64  (std::uint64_t                          i);
        void sendFloat32 (SockLib::Helper::float32_t             f);
        void sendFloat64 (SockLib::Helper::float64_t             f);
        void sendBytes   (const std::byte *bytes, std::size_t size);

        void sendBool  (bool             b);
        void sendChar  (char             c);
        void sendInt   (int              i);
        void sendFloat (float            f);
        void sendStr   (std::string_view s);

        std::int8_t                recvInt8    (void);
        std::uint8_t               recvUint8   (void);
        std::int16_t               recvInt16   (void);
        std::uint16_t              recvUint16  (void);
        std::int32_t               recvInt32   (void);
        std::uint32_t              recvUint32  (void);
        std::int64_t               recvInt64   (void);
        std::uint64_t              recvUint64  (void);
        SockLib::Helper::float32_t recvFloat32 (void);
        SockLib::Helper::float64_t recvFloat64 (void);
        std::vector<std::byte>     recvBytes   (std::uint32_t limit);

        bool        recvBool  (void);
        char        recvChar  (void);
        int         recvInt   (void);
        float       recvFloat (void);
        std::string recvStr   (std::uint32_t limit);

        static SockLib::Sock connect(const char *address, std::uint16_t port, std::size_t timeoutMS);

    private:
        void sendRawAllBytes(const std::byte *bytes, std::size_t size);
        void recvRawAllBytes(std::byte       *bytes, std::size_t size);

        Sock(SockLib::Helper::Sock new_socket);
        
        SockLib::Helper::Sock m_socket;
        
        friend class SockLib::Server;
    };
}
#endif // SOCK_LIB_SOCK
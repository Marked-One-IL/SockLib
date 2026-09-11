#ifndef SOCK_LIB_DESERIALIZER
#define SOCK_LIB_DESERIALIZER
#include <SockLib/Helper.hpp>
#include <memory>
#include <string>
#include <vector>
#include <cstddef>
#include <cstring>

namespace SockLib
{
    class Sock;

    class Deserializer
    {
    public:
        Deserializer(void) = delete;

        std::int8_t                deserializeInt8    (void);
        std::uint8_t               deserializeUint8   (void);
        std::int16_t               deserializeInt16   (void);
        std::uint16_t              deserializeUint16  (void);
        std::int32_t               deserializeInt32   (void);
        std::uint32_t              deserializeUint32  (void);
        std::int64_t               deserializeInt64   (void);
        std::uint64_t              deserializeUint64  (void);
        SockLib::Helper::float32_t deserializeFloat32 (void);
        SockLib::Helper::float64_t deserializeFloat64 (void);
        std::vector<std::byte>     deserializeBytes   (void);

        bool             deserializeBool    (void);
        char             deserializeChar    (void);
        int              deserializeInt     (void);
        float            deserializeFloat   (void);
        std::string      deserializeStrCopy (void);
        std::string_view deserializeStrView (void);

    private:
        void deserializeBytesRaw (std::byte* bytes, std::size_t size);

        Deserializer(std::size_t size, SockLib::Sock &originSock);
        std::byte *getBytes(void);

        std::unique_ptr<std::byte[]> m_bytes;
        std::size_t m_current = 0;
        std::size_t m_totalSize;
        SockLib::Sock &m_originSock;

        friend class SockLib::Sock;
    };
}
#endif // SOCK_LIB_DESERIALIZER
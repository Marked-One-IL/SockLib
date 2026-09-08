#ifndef SOCK_LIB_SERIALIZER
#define SOCK_LIB_SERIALIZER
#include <SockLib/Helper.hpp>
#include <vector>
#include <string_view>
#include <cstddef>
#include <cstdint>

namespace SockLib
{
    class Sock;

    class Serializer
    {
    public:
        Serializer(void) = default;
        void reserve(std::size_t size);
        
        // Advanced methods.

        void serializeInt8    (std::int8_t                i);
        void serializeUint8   (std::uint8_t               i);
        void serializeInt16   (std::int16_t               i);
        void serializeUint16  (std::uint16_t              i);
        void serializeInt32   (std::int32_t               i);
        void serializeUint32  (std::uint32_t              i);
        void serializeInt64   (std::int64_t               i);
        void serializeUint64  (std::uint64_t              i);
        void serializeFloat32 (SockLib::Helper::float32_t f);
        void serializeFloat64 (SockLib::Helper::float64_t f);
        void serializeBytes   (const std::byte *bytes, std::size_t size);

        // Simple methods.

        void serializeBool  (bool             b);
        void serializeChar  (char             c);
        void serializeInt   (int              i);
        void serializeFloat (float            f);
        void serializeStr   (std::string_view s);

    private:
        const std::byte *getBytes (void) const;
        std::size_t      getSize  (void) const;

        std::vector<std::byte> m_bytes;
        friend class SockLib::Sock;
    };
}
#endif // SOCK_LIB_SERIALIZER
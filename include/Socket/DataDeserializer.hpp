#pragma once
#include <Socket/Utils.hpp>
#include <memory>
#include <string>
#include <cstddef>
#include <cstring>

namespace Socket
{
    class DataDeserializer
    {
    public:
        DataDeserializer(const std::byte *bytes, std::size_t size)
        {
            this->m_bytes = std::make_unique<std::byte[]>(size);
            std::memcpy(this->m_bytes.get(), bytes, size);
            this->m_totalSize = size;
        }

        // Advanced methods.

        std::int8_t              deserializeInt8    (void);
        std::uint8_t             deserializeUint8   (void);
        std::int16_t             deserializeInt16   (void);
        std::uint16_t            deserializeUint16  (void);
        std::int32_t             deserializeInt32   (void);
        std::uint32_t            deserializeUint32  (void);
        std::int64_t             deserializeInt64   (void);
        std::uint64_t            deserializeUint64  (void);
        Socket::Utils::float32_t deserializeFloat32 (void);
        Socket::Utils::float64_t deserializeFloat64 (void);
        void                     deserializeBytes   (std::byte *bytes, std::size_t size);

        // Simple methods.

        bool        deserializeBool  (void);
        char        deserializeChar  (void);
        int         deserializeInt   (void);
        float       deserializeFloat (void);
        std::string deserializeStr   (void);

    private:
        std::unique_ptr<std::byte[]> m_bytes;
        std::size_t m_current = 0;

#ifndef NDEBUG // Only for asserting.
        std::size_t m_totalSize = 0;
#endif

        // friend class _;
    };
}
#include <Socket/Utils.hpp>
#include <bit>

namespace Socket::Utils
{
    // All integers must be little endian unlike other implementations (eg: htonl()).
    // Most machines today are like that by default.
    // Then why not flip the logic and save a couple of instructions?

    std::uint16_t normalizeUint16(std::uint16_t v)
    {
        if constexpr (std::endian::native == std::endian::big) {
            return Socket::Utils::uint16BitsSwap(v);
        }
        return v;
    }
    std::uint32_t normalizeUint32(std::uint32_t v)
    {
        if constexpr (std::endian::native == std::endian::big) {
            return Socket::Utils::uint32BitsSwap(v);
        }
        return v;
    }
    std::uint64_t normalizeUint64(std::uint64_t v)
    {
        if constexpr (std::endian::native == std::endian::big) {
            return Socket::Utils::uint64BitsSwap(v);
        }
        return v;
    }

    std::uint16_t uint16BitsSwap(std::uint16_t v)
    {
        return (v >> 8) | (v << 8);
    }
    std::uint32_t uint32BitsSwap(std::uint32_t v)
    {
        return ((v & 0x000000FFu) << 24) | 
               ((v & 0x0000FF00u) << 8)  |
               ((v & 0x00FF0000u) >> 8)  |
               ((v & 0xFF000000u) >> 24);
    }
    std::uint64_t uint64BitsSwap(std::uint64_t v)
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
}
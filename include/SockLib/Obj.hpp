#ifndef SOCK_LIB_OBJ
#define SOCK_LIB_OBJ
#include <SockLib/Helper.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <tuple>
#include <utility>
#include <limits>
#include <cstdint>
#include <cstddef>

namespace SockLib
{
    class Sock;
}

#define SOCK_LIB_OBJ_TRANSMISSION_ORDER(...) \
    friend class SockLib::Sock; \
    auto to_tuple(void) { \
        return std::tie(__VA_ARGS__); \
    } \
    auto to_tuple(void) const { \
        return std::tie(__VA_ARGS__); \
    }

namespace SockLib::Obj
{
    using Int8      = std::int8_t;
    using Uint8     = std::uint8_t;
    using Int16     = std::int16_t;
    using Uint16    = std::uint16_t;
    using Int32     = std::int32_t;
    using Uint32    = std::uint32_t;
    using Int64     = std::int64_t;
    using Uint64    = std::uint64_t;
    using Float32   = SockLib::Helper::float32_t;
    using Float64   = SockLib::Helper::float64_t;
    using Bytes     = std::vector<std::byte>;
    using BytesSpan = std::span<std::byte>; // Can be used only for sending. The receiving field must be 'SockLib::Obj::Bytes'.

    using Bool    = bool;
    using Char    = char;
    using Int     = int;
    using Float   = float;
    using Str     = std::string;
    using StrView = std::string_view; // Can be used only for sending. The receiving field must be 'SockLib::Obj::Str'.
}

#endif // SOCK_LIB_OBJ
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


// "Why not create an interface?".
// The reason I don't it's because of the limition of C++.
// 'auto' here does a lot of the heavy lifting but without it unlimited arguments (...) are not useable.
// A virtual method cannot use that 'auto' trick.
// Also it doesn't look that nice.

// Doesn't matter if it's in the public/private section (But it's recommended to put this in the private section).
// Sending all the fields is not required.
// But the amount of fields and their order must be the same on both ends.
#define SOCK_LIB_OBJ_DEFINE_PROTOCOL(...) \
    friend class SockLib::Sock; \
    auto _SockLibObjToTupleConst(void) const { \
        return std::tie(__VA_ARGS__); \
    } \
    auto _SockLibObjToTupleNoneConst(void) { \
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

    using Bool    = bool;  // On the network it's 'std::uint8_t'.
    using Char    = char;  // On the network it's 'std::uint8_t'.
    using Int     = int;   // On the network it's 'std::int32_t'.
    using Float   = float; // On the network it's 'SockLib::Helper::float32_t'.
    using Str     = std::string;
    using StrView = std::string_view; // Can be used only for sending. The receiving field must be 'SockLib::Obj::Str'.
}
#endif // SOCK_LIB_OBJ
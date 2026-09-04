#pragma once
#include <limits>
#include <cstdint>
#include <stdfloat>

// Me being overwhelmingly paranoid.
// Almost 100% of all machines support IEEE-754.
// But the C standard does not guarantee it.
static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);
static_assert(std::numeric_limits<float>::is_iec559);
static_assert(std::numeric_limits<double>::is_iec559);

namespace Socket::Utils
{
    using float32_t = float;
    using float64_t = double;

    std::uint16_t normalizeUint16 (std::uint16_t v);
    std::uint32_t normalizeUint32 (std::uint32_t v);
    std::uint64_t normalizeUint64 (std::uint64_t v);

    std::uint16_t uint16BitsSwap  (std::uint16_t v);
    std::uint32_t uint32BitsSwap  (std::uint32_t v);
    std::uint64_t uint64BitsSwap  (std::uint64_t v);
}
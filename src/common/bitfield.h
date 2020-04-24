#pragma once

#include <cstdint>
#include <cstddef>
#include <utility>
#include <climits>

template <typename T, int pos, std::size_t width = 1>
struct bitfield {
    static_assert(pos >= 0, "Bit position cannot be negative");
    static_assert(pos <= sizeof(T) * CHAR_BIT, "Bit position exceeding underlying type size");
    static_assert(width <= sizeof(T) * CHAR_BIT, "Width exceeding underlying type size");
    static_assert(width > 0, "Width must be positive");

    template <typename V>
    constexpr auto operator=(V v) {
        data &= ~mask;
        data |= v << pos;

        return *this;
    }

    operator unsigned() const { return (data & mask) >> pos; }

   private:
    T data;
    static constexpr T mask = []<std::size_t... p>(std::index_sequence<p...>) {
        return ((1 << (p + pos)) | ...);
    }
    (std::make_index_sequence<width>{});
};

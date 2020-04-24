#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "common/bitfield.h"

union regtest {
    std::uint8_t r;
    bitfield<std::uint8_t, 0> a;
    bitfield<std::uint8_t, 1> b;
    bitfield<std::uint8_t, 2, 2> c;
    bitfield<std::uint8_t, 3> d;
    bitfield<std::uint8_t, 4, 4> e;
};

TEST_CASE("Value checking") {
    regtest t;
    t.r = 0xFF;
    t.e = 0;
    REQUIRE(t.r == 0x0F);

    t.r = 0xF0;
    REQUIRE(t.e == 0b1111);

    SECTION("Overlapping members") {
        t.c = 0b11;
        REQUIRE(t.r == 0b11111100);
        REQUIRE(t.d == 0b1);
    }
}


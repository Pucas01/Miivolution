#include <catch2/catch_test_macros.hpp>
#include "RevoInternal/endian.hpp"
#include <cstdint>

TEST_CASE("Endian conversion", "[endian]") {
    SECTION("bswap16") {
        REQUIRE(revointernal::bswap16(0x1234) == 0x3412);
        REQUIRE(revointernal::bswap16(0x039d) == 0x9d03);
    }

    SECTION("bswap32") {
        REQUIRE(revointernal::bswap32(0x12345678) == 0x78563412);
        REQUIRE(revointernal::bswap32(0x0000004c) == 0x4c000000);
    }

    SECTION("readBE<uint16_t>") {
        uint8_t data[] = {0x03, 0x9d};
        uint16_t value = revointernal::readBE<uint16_t>(data);
        REQUIRE(value == 0x039d);
        REQUIRE(value == 925);
    }

    SECTION("readBE<uint32_t>") {
        uint8_t data[] = {0x00, 0x00, 0x00, 0x4c};
        uint32_t value = revointernal::readBE<uint32_t>(data);
        REQUIRE(value == 0x0000004c);
        REQUIRE(value == 76);
    }
}

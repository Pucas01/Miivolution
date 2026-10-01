#include <catch2/catch_test_macros.hpp>
#include "RVLFaceLib.h"
#include <vector>

TEST_CASE("RFL MiddleDB - Buffer size calculation", "[rfl][middledb]") {
    SECTION("Returns non-zero for valid sizes") {
        REQUIRE(RFLGetMiddleDBBufferSize(1) > 0);
        REQUIRE(RFLGetMiddleDBBufferSize(10) > 0);
        REQUIRE(RFLGetMiddleDBBufferSize(100) > 0);
    }

    SECTION("Size scales linearly with entry count") {
        u32 size1 = RFLGetMiddleDBBufferSize(1);
        u32 size10 = RFLGetMiddleDBBufferSize(10);
        u32 size100 = RFLGetMiddleDBBufferSize(100);

        REQUIRE(size10 == size1 * 10);
        REQUIRE(size100 == size1 * 100);
    }

    SECTION("Zero size returns zero") {
        REQUIRE(RFLGetMiddleDBBufferSize(0) == 0);
    }
}

TEST_CASE("RFL MiddleDB - Initialization", "[rfl][middledb]") {
    RFLMiddleDB db;
    std::vector<u8> buffer(RFLGetMiddleDBBufferSize(10));

    SECTION("Can initialize all database types") {
        RFLInitMiddleDB(&db, RFLMiddleDBType_Random, buffer.data(), 10);
        RFLInitMiddleDB(&db, RFLMiddleDBType_HiddenRandom, buffer.data(), 10);
        RFLInitMiddleDB(&db, RFLMiddleDBType_HiddenNewer, buffer.data(), 10);
        RFLInitMiddleDB(&db, RFLMiddleDBType_HiddenOlder, buffer.data(), 10);
        RFLInitMiddleDB(&db, RFLMiddleDBType_UserSet, buffer.data(), 10);
    }

    SECTION("Handles null db pointer gracefully") {
        RFLInitMiddleDB(nullptr, RFLMiddleDBType_Random, buffer.data(), 10);
    }

    SECTION("Handles null buffer pointer gracefully") {
        RFLInitMiddleDB(&db, RFLMiddleDBType_Random, nullptr, 10);
    }

    SECTION("Accepts various sizes") {
        RFLInitMiddleDB(&db, RFLMiddleDBType_Random, buffer.data(), 1);
        RFLInitMiddleDB(&db, RFLMiddleDBType_Random, buffer.data(), 10);
    }
}

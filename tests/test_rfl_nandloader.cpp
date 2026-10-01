#include <catch2/catch_test_macros.hpp>
#include "RVLFaceLib.h"
#include <vector>

TEST_CASE("RFL NANDLoader - Resource caching", "[rfl][nandloader]") {
    SECTION("Resource not cached before init") {
        REQUIRE(RFLIsResourceCached() == FALSE);
    }

    SECTION("Resource cached after init with cache") {
        std::vector<u8> workBuffer(RFLGetWorkSize(FALSE));
        std::vector<u8> resBuffer(0x10000);

        RFLErrcode result = RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);
        REQUIRE(result == RFLErrcode_Success);

        REQUIRE(RFLIsResourceCached() == TRUE);

        RFLExit();
    }

    SECTION("FreeCachedResource returns TRUE when available") {
        std::vector<u8> workBuffer(RFLGetWorkSize(FALSE));
        std::vector<u8> resBuffer(0x10000);

        RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);

        BOOL freed = RFLFreeCachedResource();
        REQUIRE(freed == TRUE);

        RFLExit();
    }

    SECTION("Resource not cached after freeing") {
        std::vector<u8> workBuffer(RFLGetWorkSize(FALSE));
        std::vector<u8> resBuffer(0x10000);

        RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);
        RFLFreeCachedResource();

        REQUIRE(RFLIsResourceCached() == FALSE);

        RFLExit();
    }
}

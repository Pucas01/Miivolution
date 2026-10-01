#include <catch2/catch_test_macros.hpp>
#include "RVLFaceLib.h"
#include <vector>
#include <cstring>

TEST_CASE("RFL System - Work size calculation", "[rfl][system]") {
    u32 standardSize = RFLGetWorkSize(FALSE);
    u32 deluxeSize = RFLGetWorkSize(TRUE);

    SECTION("Returns non-zero sizes") {
        REQUIRE(standardSize > 0);
        REQUIRE(deluxeSize > 0);
    }

    SECTION("Deluxe size is larger than standard") {
        REQUIRE(deluxeSize > standardSize);
    }

    SECTION("Sizes are reasonable (between 32KB and 1MB)") {
        REQUIRE(standardSize >= 0x8000);
        REQUIRE(standardSize <= 0x100000);
        REQUIRE(deluxeSize >= 0x8000);
        REQUIRE(deluxeSize <= 0x100000);
    }
}

TEST_CASE("RFL System - Initialization validation", "[rfl][system]") {
    SECTION("Rejects null work buffer") {
        std::vector<u8> resBuffer(0x1000);
        RFLErrcode result = RFLInitRes(nullptr, resBuffer.data(), resBuffer.size(), FALSE);
        REQUIRE(result == RFLErrcode_WrongParam);
    }

    SECTION("Rejects null resource buffer") {
        std::vector<u8> workBuffer(RFLGetWorkSize(FALSE));
        RFLErrcode result = RFLInitRes(workBuffer.data(), nullptr, 0x1000, FALSE);
        REQUIRE(result == RFLErrcode_WrongParam);
    }

    SECTION("Rejects zero resource size") {
        std::vector<u8> workBuffer(RFLGetWorkSize(FALSE));
        std::vector<u8> resBuffer(0x1000);
        RFLErrcode result = RFLInitRes(workBuffer.data(), resBuffer.data(), 0, FALSE);
        REQUIRE(result == RFLErrcode_WrongParam);
    }
}

TEST_CASE("RFL System - Initialization lifecycle", "[rfl][system]") {
    std::vector<u8> workBuffer(RFLGetWorkSize(FALSE));
    std::vector<u8> resBuffer(0x1000);

    SECTION("System not available before init") {
        REQUIRE(RFLAvailable() == FALSE);
    }

    SECTION("Successful initialization") {
        RFLErrcode result = RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);
        REQUIRE(result == RFLErrcode_Success);
        REQUIRE(RFLAvailable() == TRUE);
        RFLExit();
    }

    SECTION("System not available after exit") {
        RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);
        REQUIRE(RFLAvailable() == TRUE);
        RFLExit();
        REQUIRE(RFLAvailable() == FALSE);
    }

    SECTION("Can reinitialize after exit") {
        RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);
        RFLExit();
        RFLErrcode result = RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);
        REQUIRE(result == RFLErrcode_Success);
        REQUIRE(RFLAvailable() == TRUE);
        RFLExit();
    }
}

TEST_CASE("RFL System - Async operations", "[rfl][system]") {
    std::vector<u8> workBuffer(RFLGetWorkSize(FALSE));
    std::vector<u8> resBuffer(0x1000);

    SECTION("InitResAsync returns immediately") {
        RFLErrcode result = RFLInitResAsync(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);
        REQUIRE((result == RFLErrcode_Success || result == RFLErrcode_Busy));

        if (result == RFLErrcode_Busy) {
            RFLErrcode waitResult = RFLWaitAsync();
            REQUIRE(waitResult == RFLErrcode_Success);
        }

        REQUIRE(RFLAvailable() == TRUE);
        RFLExit();
    }

    SECTION("GetAsyncStatus after successful init") {
        RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);
        RFLErrcode status = RFLGetAsyncStatus();
        REQUIRE(status == RFLErrcode_Success);
        RFLExit();
    }

    SECTION("GetLastReason returns valid value") {
        RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);
        s32 reason = RFLGetLastReason();
        REQUIRE(reason >= 0);
        RFLExit();
    }
}

TEST_CASE("RFL System - Multiple exit calls are safe", "[rfl][system]") {
    RFLExit();
    RFLExit();
    REQUIRE(RFLAvailable() == FALSE);
}

TEST_CASE("RFL System - Deluxe texture mode", "[rfl][system]") {
    std::vector<u8> workBuffer(RFLGetWorkSize(TRUE));
    std::vector<u8> resBuffer(0x2000);

    RFLErrcode result = RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), TRUE);
    REQUIRE(result == RFLErrcode_Success);
    REQUIRE(RFLAvailable() == TRUE);
    RFLExit();
}

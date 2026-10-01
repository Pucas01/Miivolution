#include <catch2/catch_test_macros.hpp>
#include "RVLFaceLib.h"
#include <vector>

TEST_CASE("RFL Resource - Parse header structure", "[rfl][resource]") {
    std::vector<u8> workBuffer(RFLGetWorkSize(FALSE));
    std::vector<u8> fakeRes(0x1000, 0);

    fakeRes[2] = 0x05;
    fakeRes[3] = 0x00;

    RFLErrcode result = RFLInitRes(workBuffer.data(), fakeRes.data(), fakeRes.size(), FALSE);
    REQUIRE(result == RFLErrcode_Success);

    RFLExit();
}

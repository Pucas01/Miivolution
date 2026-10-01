#include <catch2/catch_test_macros.hpp>
#include "RVLFaceLib.h"
#include "../src/RVLFaceLib/Model/model_internal.hpp"
#include <vector>
#include <cstring>

TEST_CASE("RFL Shape - Vertex size macros", "[rfl][shape]") {
    SECTION("Vertex size macros compute correctly") {
        REQUIRE(SIZE_VTX_POS(10) == 10 * 3 * sizeof(s16));
        REQUIRE(SIZE_VTX_NRM(10) == 10 * 3 * sizeof(s16));
        REQUIRE(SIZE_VTX_TXC(10) == 10 * 2 * sizeof(s16));
    }

    SECTION("Vertex macros round-trip correctly") {
        REQUIRE(NUM_VTX_POS(SIZE_VTX_POS(10)) == 10);
        REQUIRE(NUM_VTX_NRM(SIZE_VTX_NRM(10)) == 10);
        REQUIRE(NUM_VTX_TXC(SIZE_VTX_TXC(10)) == 10);
    }
}

TEST_CASE("RFL Shape - Texture helper function", "[rfl][shape]") {
    SECTION("getTexImage computes correct offset") {
        std::vector<u8> buffer(sizeof(rvlfacelib::Texture) + 1024);
        auto* tex = reinterpret_cast<rvlfacelib::Texture*>(buffer.data());
        tex->imageOfs = sizeof(rvlfacelib::Texture);

        void* image = rvlfacelib::getTexImage(tex);
        REQUIRE(image == buffer.data() + sizeof(rvlfacelib::Texture));
    }

    SECTION("getTexImage handles different offsets") {
        std::vector<u8> buffer(2048);
        auto* tex = reinterpret_cast<rvlfacelib::Texture*>(buffer.data());

        tex->imageOfs = 100;
        REQUIRE(rvlfacelib::getTexImage(tex) == buffer.data() + 100);

        tex->imageOfs = 500;
        REQUIRE(rvlfacelib::getTexImage(tex) == buffer.data() + 500);
    }
}

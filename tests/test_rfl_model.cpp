#include <catch2/catch_test_macros.hpp>
#include "RVLFaceLib.h"
#include <vector>

TEST_CASE("RFL Model - Buffer size calculation", "[rfl][model]") {
    SECTION("Returns non-zero for valid resolutions") {
        REQUIRE(RFLGetModelBufferSize(RFLResolution_64, RFLExpFlag_Normal) > 0);
        REQUIRE(RFLGetModelBufferSize(RFLResolution_128, RFLExpFlag_Normal) > 0);
        REQUIRE(RFLGetModelBufferSize(RFLResolution_256, RFLExpFlag_Normal) > 0);
    }

    SECTION("Larger resolutions require more memory") {
        u32 size64 = RFLGetModelBufferSize(RFLResolution_64, RFLExpFlag_Normal);
        u32 size128 = RFLGetModelBufferSize(RFLResolution_128, RFLExpFlag_Normal);
        u32 size256 = RFLGetModelBufferSize(RFLResolution_256, RFLExpFlag_Normal);

        REQUIRE(size128 > size64);
        REQUIRE(size256 > size128);
    }

    SECTION("More expressions require more memory") {
        u32 singleExpr = RFLGetModelBufferSize(RFLResolution_128, RFLExpFlag_Normal);
        u32 multiExpr = RFLGetModelBufferSize(RFLResolution_128,
            RFLExpFlag_Normal | RFLExpFlag_Smile | RFLExpFlag_Anger);

        REQUIRE(multiExpr > singleExpr);
    }

    SECTION("Mipmapped resolutions require more memory") {
        u32 size128 = RFLGetModelBufferSize(RFLResolution_128, RFLExpFlag_Normal);
        u32 size128M = RFLGetModelBufferSize(RFLResolution_128M, RFLExpFlag_Normal);

        REQUIRE(size128M > size128);
    }
}

TEST_CASE("RFL Model - Favorite colors", "[rfl][model]") {
    SECTION("Returns valid colors for all favorite color enums") {
        for (int i = 0; i < RFLFavoriteColor_Max; i++) {
            GXColor color = RFLGetFavoriteColor(static_cast<RFLFavoriteColor>(i));
            REQUIRE(color.a == 255);
        }
    }

    SECTION("Different colors return different values") {
        GXColor red = RFLGetFavoriteColor(RFLFavoriteColor_Red);
        GXColor blue = RFLGetFavoriteColor(RFLFavoriteColor_Blue);

        REQUIRE((red.r != blue.r || red.g != blue.g || red.b != blue.b));
    }

    SECTION("Red color has high red component") {
        GXColor red = RFLGetFavoriteColor(RFLFavoriteColor_Red);
        REQUIRE(red.r > 100);
    }

    SECTION("Blue color has high blue component") {
        GXColor blue = RFLGetFavoriteColor(RFLFavoriteColor_Blue);
        REQUIRE(blue.b > 100);
    }
}

TEST_CASE("RFL Model - Coordinate system", "[rfl][model]") {
    SECTION("Can set coordinate system") {
        RFLSetCoordinate(RFLCoordinateType_X, RFLCoordinateType_Y);
        RFLSetCoordinate(RFLCoordinateType_Y, RFLCoordinateType_Z);
        RFLSetCoordinate(RFLCoordinateType_Z, RFLCoordinateType_X);
    }

    SECTION("Can set reversed coordinate system") {
        RFLSetCoordinate(RFLCoordinateType_RevX, RFLCoordinateType_Y);
        RFLSetCoordinate(RFLCoordinateType_X, RFLCoordinateType_RevY);
    }
}

TEST_CASE("RFL Model - Character model lifecycle", "[rfl][model]") {
    std::vector<u8> workBuffer(RFLGetWorkSize(FALSE));
    std::vector<u8> resBuffer(0x1000);
    RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);

    SECTION("InitCharModel requires initialization") {
        RFLCharModel model;
        u32 bufSize = RFLGetModelBufferSize(RFLResolution_128, RFLExpFlag_Normal);
        std::vector<u8> modelBuf(bufSize);

        RFLMiddleDB db;
        std::vector<u8> dbBuf(RFLGetMiddleDBBufferSize(10));
        RFLInitMiddleDB(&db, RFLMiddleDBType_Random, dbBuf.data(), 10);

        RFLErrcode result = RFLInitCharModel(&model, RFLDataSource_Official, &db, 0,
            modelBuf.data(), RFLResolution_128, RFLExpFlag_Normal);

        REQUIRE((result == RFLErrcode_Success || result == RFLErrcode_NotAvailable ||
                 result == RFLErrcode_DBNodata));
    }

    RFLExit();
}

TEST_CASE("RFL Model - Expression management", "[rfl][model]") {
    SECTION("Default expression getter works without model") {
        RFLCharModel model = {};
        RFLExpression expr = RFLGetExpression(&model);
        REQUIRE(expr >= RFLExp_Normal);
        REQUIRE(expr < RFLExp_Max);
    }

    SECTION("Can set and get expressions") {
        std::vector<u8> workBuffer(RFLGetWorkSize(FALSE));
        std::vector<u8> resBuffer(0x1000);
        RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);

        RFLCharModel model;
        u32 bufSize = RFLGetModelBufferSize(RFLResolution_128, RFLExpFlag_Normal);
        std::vector<u8> modelBuf(bufSize);

        RFLMiddleDB db;
        std::vector<u8> dbBuf(RFLGetMiddleDBBufferSize(10));
        RFLInitMiddleDB(&db, RFLMiddleDBType_Random, dbBuf.data(), 10);

        RFLInitCharModel(&model, RFLDataSource_Official, &db, 0,
            modelBuf.data(), RFLResolution_128, RFLExpFlag_Normal);

        RFLSetExpression(&model, RFLExp_Smile);
        RFLExpression expr = RFLGetExpression(&model);
        REQUIRE(expr == RFLExp_Smile);

        RFLExit();
    }

    SECTION("All expression values are valid") {
        std::vector<u8> workBuffer(RFLGetWorkSize(FALSE));
        std::vector<u8> resBuffer(0x1000);
        RFLInitRes(workBuffer.data(), resBuffer.data(), resBuffer.size(), FALSE);

        RFLCharModel model;
        u32 bufSize = RFLGetModelBufferSize(RFLResolution_128, RFLExpFlag_Normal);
        std::vector<u8> modelBuf(bufSize);

        RFLMiddleDB db;
        std::vector<u8> dbBuf(RFLGetMiddleDBBufferSize(10));
        RFLInitMiddleDB(&db, RFLMiddleDBType_Random, dbBuf.data(), 10);

        RFLInitCharModel(&model, RFLDataSource_Official, &db, 0,
            modelBuf.data(), RFLResolution_128, RFLExpFlag_Normal);

        for (int i = RFLExp_Normal; i < RFLExp_Max; i++) {
            RFLSetExpression(&model, static_cast<RFLExpression>(i));
            RFLExpression result = RFLGetExpression(&model);
            REQUIRE(result == i);
        }

        RFLExit();
    }
}

#pragma once

#include "Miivolution/database.hpp"
#include "Miivolution/mii.hpp"
#include "RVLFaceLib.h"
#include <filesystem>
#include <vector>
#include <cstring>

namespace fs = std::filesystem;

namespace test_helpers {

inline fs::path testTempDir;

inline char* testPrefPathLocal(const char*, const char*) {
    return strdup(testTempDir.string().c_str());
}

inline void testFreePathLocal(void* ptr) {
    free(ptr);
}

struct MiivolutionAPIFixture {
    MiivolutionAPIFixture() {
        static int counter = 0;
        testTempDir = fs::temp_directory_path() / ("miivolution_test_" + std::to_string(++counter));
        fs::create_directories(testTempDir);

        miivolution::database::setPrefPath({testPrefPathLocal, testFreePathLocal});

        u32 workSize = RFLGetWorkSize(FALSE);
        u32 resSize = 1024 * 1024;
        workBuffer.resize(workSize);
        resBuffer.resize(resSize);
        RFLInitRes(workBuffer.data(), resBuffer.data(), resSize, FALSE);
    }

    ~MiivolutionAPIFixture() {
        RFLExit();

        if (fs::exists(testTempDir)) {
            fs::remove_all(testTempDir);
        }
    }

    std::vector<u8> workBuffer;
    std::vector<u8> resBuffer;
};

inline miivolution::mii::MII_DATA_STRUCT createTestMii(const char* name, u8 height = 64, u8 build = 64) {
    static int createIDCounter = 0;
    miivolution::mii::MII_DATA_STRUCT mii{};

    mii.sex = 0;
    mii.birthMonth = 3;
    mii.birthDay = 15;
    mii.favoriteColor = 4;
    mii.favorite = 0;

    for (int i = 0; i < RFL_NAME_LEN && name[i]; i++) {
        mii.name[i] = static_cast<u16>(name[i]);
    }

    mii.height = height;
    mii.build = build;

    int counter = createIDCounter++;
    for (int i = 0; i < 8; i++) {
        mii.createID.data[i] = static_cast<u8>((counter + i * 17) & 0xFF);
    }

    mii.faceType = 2;
    mii.faceColor = 1;
    mii.faceTex = 0;
    mii.localonly = 0;
    mii.type = 0;

    mii.hairType = 5;
    mii.hairColor = 2;
    mii.hairFlip = 0;

    mii.eyebrowType = 3;
    mii.eyebrowRotate = 0;
    mii.eyebrowColor = 2;
    mii.eyebrowScale = 4;
    mii.eyebrowY = 10;
    mii.eyebrowX = 2;

    mii.eyeType = 4;
    mii.eyeRotate = 0;
    mii.eyeY = 12;
    mii.eyeColor = 0;
    mii.eyeScale = 4;
    mii.eyeX = 2;

    mii.noseType = 1;
    mii.noseScale = 4;
    mii.noseY = 9;

    mii.mouthType = 5;
    mii.mouthColor = 1;
    mii.mouthScale = 4;
    mii.mouthY = 13;

    mii.glassType = 0;
    mii.glassColor = 0;
    mii.glassScale = 4;
    mii.glassY = 10;

    mii.mustacheType = 0;
    mii.beardType = 0;
    mii.beardColor = 2;
    mii.beardScale = 4;
    mii.beardY = 10;

    mii.moleType = 0;
    mii.moleScale = 4;
    mii.moleY = 20;
    mii.moleX = 2;

    const char* creator = "Test";
    for (int i = 0; i < RFL_CREATOR_LEN && creator[i]; i++) {
        mii.creatorName[i] = static_cast<u16>(creator[i]);
    }

    return mii;
}

inline void clearDatabase() {
    miivolution::database::loadDatabase();
    for (u16 i = 0; i < RFL_DB_CHAR_MAX; i++) {
        miivolution::database::deleteMii(i);
    }
}

} // namespace test_helpers

#include <catch2/catch_test_macros.hpp>
#include "test_helpers.hpp"
#include "Miivolution/mii.hpp"
#include <cstring>

using namespace test_helpers;

TEST_CASE("Mii serialization - Basic operations", "[mii][serialization]") {
    MiivolutionAPIFixture fixture;

    SECTION("Can serialize a Mii") {
        auto testMii = createTestMii("Serialize", 100, 25);
        u8 buffer[0x4A] = {0};

        REQUIRE(miivolution::mii::serializeMii(testMii, buffer, sizeof(buffer)));
    }

    SECTION("Serialized data is non-zero") {
        auto testMii = createTestMii("NonZero");
        u8 buffer[0x4A] = {0};

        REQUIRE(miivolution::mii::serializeMii(testMii, buffer, sizeof(buffer)));

        bool hasData = false;
        for (size_t i = 0; i < sizeof(buffer); i++) {
            if (buffer[i] != 0) {
                hasData = true;
                break;
            }
        }
        REQUIRE(hasData);
    }

    SECTION("Serialize rejects null buffer") {
        auto testMii = createTestMii("Invalid");
        REQUIRE_FALSE(miivolution::mii::serializeMii(testMii, nullptr, 0x4A));
    }

    SECTION("Serialize rejects buffer too small") {
        auto testMii = createTestMii("TooSmall");
        u8 tooSmall[10] = {0};
        REQUIRE_FALSE(miivolution::mii::serializeMii(testMii, tooSmall, sizeof(tooSmall)));
    }

    SECTION("Serialize with exact size works") {
        auto testMii = createTestMii("ExactSize");
        u8 buffer[0x4A] = {0};
        REQUIRE(miivolution::mii::serializeMii(testMii, buffer, 0x4A));
    }
}

TEST_CASE("Mii deserialization - Basic operations", "[mii][serialization]") {
    MiivolutionAPIFixture fixture;

    SECTION("Can deserialize a Mii") {
        auto original = createTestMii("Deserialize");
        u8 buffer[0x4A] = {0};

        miivolution::mii::serializeMii(original, buffer, sizeof(buffer));

        miivolution::mii::MII_DATA_STRUCT deserialized{};
        REQUIRE(miivolution::mii::deserializeMii(buffer, sizeof(buffer), deserialized));
    }

    SECTION("Deserialize rejects null buffer") {
        miivolution::mii::MII_DATA_STRUCT mii{};
        REQUIRE_FALSE(miivolution::mii::deserializeMii(nullptr, 0x4A, mii));
    }

    SECTION("Deserialize rejects buffer too small") {
        miivolution::mii::MII_DATA_STRUCT mii{};
        u8 buffer[5] = {0};
        REQUIRE_FALSE(miivolution::mii::deserializeMii(buffer, sizeof(buffer), mii));
    }

    SECTION("Deserialize with exact size works") {
        auto original = createTestMii("ExactSize");
        u8 buffer[0x4A] = {0};
        miivolution::mii::serializeMii(original, buffer, sizeof(buffer));

        miivolution::mii::MII_DATA_STRUCT deserialized{};
        REQUIRE(miivolution::mii::deserializeMii(buffer, 0x4A, deserialized));
    }
}

TEST_CASE("Mii roundtrip serialization", "[mii][serialization]") {
    MiivolutionAPIFixture fixture;

    SECTION("Basic fields survive roundtrip") {
        auto original = createTestMii("Roundtrip", 75, 90);
        u8 buffer[0x4A] = {0};

        REQUIRE(miivolution::mii::serializeMii(original, buffer, sizeof(buffer)));

        miivolution::mii::MII_DATA_STRUCT deserialized{};
        REQUIRE(miivolution::mii::deserializeMii(buffer, sizeof(buffer), deserialized));

        REQUIRE(deserialized.height == 75);
        REQUIRE(deserialized.build == 90);
        REQUIRE(deserialized.sex == original.sex);
        REQUIRE(deserialized.birthMonth == original.birthMonth);
        REQUIRE(deserialized.birthDay == original.birthDay);
        REQUIRE(deserialized.favoriteColor == original.favoriteColor);
    }

    SECTION("Name survives roundtrip") {
        auto original = createTestMii("TestName");
        u8 buffer[0x4A] = {0};

        miivolution::mii::serializeMii(original, buffer, sizeof(buffer));

        miivolution::mii::MII_DATA_STRUCT deserialized{};
        miivolution::mii::deserializeMii(buffer, sizeof(buffer), deserialized);

        for (int i = 0; i < RFL_NAME_LEN; i++) {
            REQUIRE(deserialized.name[i] == original.name[i]);
        }
    }

    SECTION("CreateID survives roundtrip") {
        auto original = createTestMii("IDTest");
        for (int i = 0; i < 8; i++) {
            original.createID.data[i] = static_cast<u8>(0xAB + i);
        }

        u8 buffer[0x4A] = {0};
        miivolution::mii::serializeMii(original, buffer, sizeof(buffer));

        miivolution::mii::MII_DATA_STRUCT deserialized{};
        miivolution::mii::deserializeMii(buffer, sizeof(buffer), deserialized);

        for (int i = 0; i < 8; i++) {
            REQUIRE(deserialized.createID.data[i] == original.createID.data[i]);
        }
    }

    SECTION("Facial features survive roundtrip") {
        auto original = createTestMii("Features");
        u8 buffer[0x4A] = {0};

        miivolution::mii::serializeMii(original, buffer, sizeof(buffer));

        miivolution::mii::MII_DATA_STRUCT deserialized{};
        miivolution::mii::deserializeMii(buffer, sizeof(buffer), deserialized);

        REQUIRE(deserialized.hairType == original.hairType);
        REQUIRE(deserialized.hairColor == original.hairColor);
        REQUIRE(deserialized.eyeType == original.eyeType);
        REQUIRE(deserialized.eyeColor == original.eyeColor);
        REQUIRE(deserialized.eyebrowType == original.eyebrowType);
        REQUIRE(deserialized.noseType == original.noseType);
        REQUIRE(deserialized.mouthType == original.mouthType);
        REQUIRE(deserialized.glassType == original.glassType);
    }

    SECTION("Edge values survive roundtrip") {
        auto original = createTestMii("EdgeCase");
        original.height = 0;
        original.build = 127;
        original.hairType = 127;
        original.eyeType = 63;

        u8 buffer[0x4A] = {0};
        miivolution::mii::serializeMii(original, buffer, sizeof(buffer));

        miivolution::mii::MII_DATA_STRUCT deserialized{};
        miivolution::mii::deserializeMii(buffer, sizeof(buffer), deserialized);

        REQUIRE(deserialized.height == 0);
        REQUIRE(deserialized.build == 127);
        REQUIRE(deserialized.hairType == 127);
        REQUIRE(deserialized.eyeType == 63);
    }
}

TEST_CASE("Guest Miis", "[mii][guest]") {
    MiivolutionAPIFixture fixture;

    SECTION("Can get all 6 guest Miis") {
        for (u16 i = 0; i < 6; i++) {
            miivolution::mii::MII_DATA_STRUCT guest{};
            REQUIRE(miivolution::mii::getGuestMii(i, guest));
        }
    }

    SECTION("Guest Miis have valid data") {
        miivolution::mii::MII_DATA_STRUCT guest{};
        REQUIRE(miivolution::mii::getGuestMii(0, guest));

        REQUIRE(guest.height == 64);
        REQUIRE(guest.build == 64);
    }

    SECTION("Guest Miis have names") {
        miivolution::mii::MII_DATA_STRUCT guest{};
        REQUIRE(miivolution::mii::getGuestMii(0, guest));

        bool hasName = false;
        for (int i = 0; i < RFL_NAME_LEN; i++) {
            if (guest.name[i] != 0) {
                hasName = true;
                break;
            }
        }
        REQUIRE(hasName);
    }

    SECTION("Different guest Miis have different data") {
        miivolution::mii::MII_DATA_STRUCT guest0{};
        miivolution::mii::MII_DATA_STRUCT guest1{};

        REQUIRE(miivolution::mii::getGuestMii(0, guest0));
        REQUIRE(miivolution::mii::getGuestMii(1, guest1));

        bool areDifferent = (guest0.hairType != guest1.hairType ||
                             guest0.eyeType != guest1.eyeType ||
                             guest0.faceType != guest1.faceType);
        REQUIRE(areDifferent);
    }

    SECTION("Invalid guest index returns false") {
        miivolution::mii::MII_DATA_STRUCT guest{};
        REQUIRE_FALSE(miivolution::mii::getGuestMii(6, guest));
        REQUIRE_FALSE(miivolution::mii::getGuestMii(100, guest));
    }

    SECTION("Guest Miis are serializable") {
        miivolution::mii::MII_DATA_STRUCT guest{};
        REQUIRE(miivolution::mii::getGuestMii(0, guest));

        u8 buffer[0x4A] = {0};
        REQUIRE(miivolution::mii::serializeMii(guest, buffer, sizeof(buffer)));
    }
}

TEST_CASE("Mii serialization - Multiple roundtrips", "[mii][serialization]") {
    MiivolutionAPIFixture fixture;

    SECTION("Data remains stable after multiple roundtrips") {
        auto original = createTestMii("Stable", 88, 77);
        miivolution::mii::MII_DATA_STRUCT current = original;

        for (int round = 0; round < 5; round++) {
            u8 buffer[0x4A] = {0};
            REQUIRE(miivolution::mii::serializeMii(current, buffer, sizeof(buffer)));

            miivolution::mii::MII_DATA_STRUCT next{};
            REQUIRE(miivolution::mii::deserializeMii(buffer, sizeof(buffer), next));

            current = next;
        }

        REQUIRE(current.height == original.height);
        REQUIRE(current.build == original.build);
        REQUIRE(current.hairType == original.hairType);
    }
}

#include <catch2/catch_test_macros.hpp>
#include "test_helpers.hpp"
#include "Miivolution/database.hpp"

using namespace test_helpers;

TEST_CASE("Database - Basic loading and state", "[database]") {
    MiivolutionAPIFixture fixture;

    SECTION("Database can be loaded") {
        REQUIRE(miivolution::database::loadDatabase());
        REQUIRE(miivolution::database::isLoaded());
    }

    SECTION("Database path is accessible") {
        miivolution::database::loadDatabase();
        fs::path dbPath = miivolution::database::getDatabasePath();
        REQUIRE(!dbPath.empty());
        REQUIRE(dbPath.parent_path().string().find("miivolution_test") != std::string::npos);
    }

    SECTION("Empty database starts with zero Miis") {
        miivolution::database::createEmptyDatabase();
        REQUIRE(miivolution::database::getMiiCount() == 0);
    }
}

TEST_CASE("Database - Add and retrieve Miis", "[database]") {
    MiivolutionAPIFixture fixture;
    miivolution::database::loadDatabase();
    clearDatabase();

    SECTION("Can add a Mii to the database") {
        auto testMii = createTestMii("Alice");
        u16 index = 0;

        REQUIRE(miivolution::database::addMii(testMii, &index));
        REQUIRE(miivolution::database::getMiiCount() == 1);
    }

    SECTION("Can retrieve added Mii") {
        auto testMii = createTestMii("Bob", 80, 50);
        u16 index = 0;
        miivolution::database::addMii(testMii, &index);

        miivolution::mii::MII_DATA_STRUCT retrieved{};
        REQUIRE(miivolution::database::getMii(index, retrieved));

        REQUIRE(retrieved.height == 80);
        REQUIRE(retrieved.build == 50);
        REQUIRE(retrieved.name[0] == u'B');
        REQUIRE(retrieved.name[1] == u'o');
        REQUIRE(retrieved.name[2] == u'b');
    }

    SECTION("Can set Mii at specific index") {
        auto mii1 = createTestMii("First");
        REQUIRE(miivolution::database::setMii(0, mii1));

        miivolution::mii::MII_DATA_STRUCT retrieved{};
        REQUIRE(miivolution::database::getMii(0, retrieved));
        REQUIRE(retrieved.name[0] == u'F');
    }

    SECTION("Can add multiple Miis") {
        auto mii1 = createTestMii("Alice");
        auto mii2 = createTestMii("Bob");
        auto mii3 = createTestMii("Charlie");

        REQUIRE(miivolution::database::addMii(mii1));
        REQUIRE(miivolution::database::addMii(mii2));
        REQUIRE(miivolution::database::addMii(mii3));

        REQUIRE(miivolution::database::getMiiCount() == 3);
    }

    SECTION("getMii rejects out-of-bounds index") {
        miivolution::mii::MII_DATA_STRUCT mii{};
        REQUIRE_FALSE(miivolution::database::getMii(10000, mii));
    }

    SECTION("setMii rejects out-of-bounds index") {
        auto testMii = createTestMii("Invalid");
        REQUIRE_FALSE(miivolution::database::setMii(10000, testMii));
    }
}

TEST_CASE("Database - Delete Miis", "[database]") {
    MiivolutionAPIFixture fixture;
    miivolution::database::loadDatabase();
    clearDatabase();

    SECTION("Can delete a Mii") {
        auto testMii = createTestMii("ToDelete");
        u16 index = 0;
        miivolution::database::addMii(testMii, &index);

        REQUIRE(miivolution::database::getMiiCount() == 1);
        REQUIRE(miivolution::database::deleteMii(index));
        REQUIRE(miivolution::database::getMiiCount() == 0);
    }

    SECTION("Deleting clears the slot") {
        auto testMii = createTestMii("Test");
        u16 index = 0;
        miivolution::database::addMii(testMii, &index);
        miivolution::database::deleteMii(index);

        miivolution::mii::MII_DATA_STRUCT retrieved{};
        miivolution::database::getMii(index, retrieved);

        bool isEmpty = true;
        for (int i = 0; i < RFL_NAME_LEN; i++) {
            if (retrieved.name[i] != 0) {
                isEmpty = false;
                break;
            }
        }
        REQUIRE(isEmpty);
    }

    SECTION("Can delete and re-add to same slot") {
        auto mii1 = createTestMii("First");
        u16 index = 0;
        miivolution::database::addMii(mii1, &index);
        u16 firstIndex = index;

        miivolution::database::deleteMii(firstIndex);

        auto mii2 = createTestMii("Second");
        miivolution::database::addMii(mii2, &index);

        REQUIRE(index == firstIndex);
    }

    SECTION("deleteMii rejects out-of-bounds index") {
        REQUIRE_FALSE(miivolution::database::deleteMii(10000));
    }
}

TEST_CASE("Database - Find operations", "[database]") {
    MiivolutionAPIFixture fixture;
    miivolution::database::loadDatabase();
    clearDatabase();

    SECTION("Find empty slot") {
        s32 slot = miivolution::database::findEmptySlot();
        REQUIRE(slot == 0);

        auto testMii = createTestMii("First");
        miivolution::database::setMii(0, testMii);

        slot = miivolution::database::findEmptySlot();
        REQUIRE(slot == 1);
    }

    SECTION("Find empty slot with gaps") {
        auto mii1 = createTestMii("Slot0");
        auto mii2 = createTestMii("Slot2");

        miivolution::database::setMii(0, mii1);
        miivolution::database::setMii(2, mii2);

        s32 slot = miivolution::database::findEmptySlot();
        REQUIRE(slot == 1);
    }

    SECTION("Find Mii by CreateID") {
        auto testMii = createTestMii("FindMe");
        for (int i = 0; i < 8; i++) {
            testMii.createID.data[i] = static_cast<u8>(0xAA + i);
        }

        u16 index = 0;
        miivolution::database::addMii(testMii, &index);

        s32 foundIndex = miivolution::database::findMiiByCreateID(testMii.createID);
        REQUIRE(foundIndex >= 0);
        REQUIRE(foundIndex == index);
    }

    SECTION("FindMiiByCreateID returns -1 for non-existent") {
        RFLCreateID fakeID{};
        for (int i = 0; i < 8; i++) {
            fakeID.data[i] = 0xFF;
        }

        s32 result = miivolution::database::findMiiByCreateID(fakeID);
        REQUIRE(result == -1);
    }

    SECTION("Find Mii by name") {
        auto mii1 = createTestMii("Alice");
        auto mii2 = createTestMii("Bob");
        auto mii3 = createTestMii("Alice");

        miivolution::database::addMii(mii1);
        miivolution::database::addMii(mii2);
        miivolution::database::addMii(mii3);

        auto aliceIndices = miivolution::database::findMiisByName("Alice");
        REQUIRE(aliceIndices.size() == 2);

        auto bobIndices = miivolution::database::findMiisByName("Bob");
        REQUIRE(bobIndices.size() == 1);

        auto nonExistent = miivolution::database::findMiisByName("Charlie");
        REQUIRE(nonExistent.empty());
    }

    SECTION("Find by name with empty string returns empty") {
        auto testMii = createTestMii("Test");
        miivolution::database::addMii(testMii);

        auto result = miivolution::database::findMiisByName("");
        REQUIRE(result.empty());
    }

    SECTION("Find by name is case-sensitive") {
        auto mii = createTestMii("Test");
        miivolution::database::addMii(mii);

        auto upperResult = miivolution::database::findMiisByName("TEST");
        REQUIRE(upperResult.empty());
    }
}

TEST_CASE("Database - Save operations", "[database]") {
    MiivolutionAPIFixture fixture;
    miivolution::database::loadDatabase();

    SECTION("Save database returns true when loaded") {
        REQUIRE(miivolution::database::saveDatabase());
    }

    SECTION("Database file is created after adding Mii") {
        auto testMii = createTestMii("Persistent");
        miivolution::database::addMii(testMii);
        miivolution::database::saveDatabase();

        fs::path dbPath = miivolution::database::getDatabasePath();
        REQUIRE(fs::exists(dbPath));
    }
}

TEST_CASE("Database - CRC16 calculation", "[database]") {
    MiivolutionAPIFixture fixture;

    SECTION("CRC16 is consistent") {
        const char* testData = "Hello, World!";
        u16 crc1 = miivolution::database::calculateCRC16(testData, strlen(testData));
        u16 crc2 = miivolution::database::calculateCRC16(testData, strlen(testData));

        REQUIRE(crc1 == crc2);
    }

    SECTION("Different data produces different CRC") {
        const char* data1 = "Test1";
        const char* data2 = "Test2";

        u16 crc1 = miivolution::database::calculateCRC16(data1, strlen(data1));
        u16 crc2 = miivolution::database::calculateCRC16(data2, strlen(data2));

        REQUIRE(crc1 != crc2);
    }

    SECTION("CRC16 handles empty data") {
        u16 crc = miivolution::database::calculateCRC16("", 0);
        REQUIRE(crc == 0);
    }
}

TEST_CASE("Database - Temp directory cleanup", "[database][cleanup]") {
    {
        MiivolutionAPIFixture fixture;
        miivolution::database::loadDatabase();

        auto testMii = createTestMii("Cleanup");
        miivolution::database::addMii(testMii);
        miivolution::database::saveDatabase();

        REQUIRE(fs::exists(testTempDir));
    }

    REQUIRE_FALSE(fs::exists(testTempDir));
}

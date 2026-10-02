#include <catch2/catch_test_macros.hpp>
#include "test_helpers.hpp"
#include "Miivolution/storage.hpp"
#include "Miivolution/database.hpp"
#include <fstream>

using namespace test_helpers;

TEST_CASE("Storage - Export Mii by struct", "[storage][export]") {
    MiivolutionAPIFixture fixture;
    miivolution::database::loadDatabase();

    fs::create_directories(testTempDir / "miixports");

    SECTION("Can export Mii with explicit path") {
        auto testMii = createTestMii("Export");

        fs::path exportPath = testTempDir / "miixports" / "test_export.mii";
        REQUIRE(miivolution::storage::exportMii(testMii, exportPath));
        REQUIRE(fs::exists(exportPath));
    }

    SECTION("Exported file has data") {
        auto testMii = createTestMii("HasData");

        fs::path exportPath = testTempDir / "miixports" / "data_export.mii";
        miivolution::storage::exportMii(testMii, exportPath);

        REQUIRE(fs::file_size(exportPath) > 0);
    }

    SECTION("Export with default path") {
        auto testMii = createTestMii("DefaultPath");
        REQUIRE(miivolution::storage::exportMii(testMii, std::nullopt));

        auto exports = miivolution::storage::getMiixports();
        REQUIRE(!exports.empty());
    }

    SECTION("Can export multiple Miis") {
        auto mii1 = createTestMii("Export1");
        auto mii2 = createTestMii("Export2");
        auto mii3 = createTestMii("Export3");

        fs::path path1 = testTempDir / "miixports" / "mii1.mii";
        fs::path path2 = testTempDir / "miixports" / "mii2.mii";
        fs::path path3 = testTempDir / "miixports" / "mii3.mii";

        REQUIRE(miivolution::storage::exportMii(mii1, path1));
        REQUIRE(miivolution::storage::exportMii(mii2, path2));
        REQUIRE(miivolution::storage::exportMii(mii3, path3));

        REQUIRE(fs::exists(path1));
        REQUIRE(fs::exists(path2));
        REQUIRE(fs::exists(path3));
    }
}

TEST_CASE("Storage - Export Mii by index", "[storage][export]") {
    MiivolutionAPIFixture fixture;
    miivolution::database::loadDatabase();

    fs::create_directories(testTempDir / "miixports");

    SECTION("Can export Mii from database by index") {
        auto testMii = createTestMii("IndexExport");
        u16 index = 0;
        miivolution::database::addMii(testMii, &index);

        fs::path exportPath = testTempDir / "miixports" / "index_export.mii";
        REQUIRE(miivolution::storage::exportMii(index, exportPath));
        REQUIRE(fs::exists(exportPath));
    }

    SECTION("Export invalid index fails") {
        fs::path exportPath = testTempDir / "miixports" / "invalid.mii";
        REQUIRE_FALSE(miivolution::storage::exportMii(static_cast<u16>(10000), exportPath));
    }
}

TEST_CASE("Storage - Export Mii by CreateID", "[storage][export]") {
    MiivolutionAPIFixture fixture;
    miivolution::database::loadDatabase();

    fs::create_directories(testTempDir / "miixports");

    SECTION("Can export Mii from database by CreateID") {
        auto testMii = createTestMii("CreateIDExport");
        for (int i = 0; i < 8; i++) {
            testMii.createID.data[i] = static_cast<u8>(0xCC + i);
        }

        miivolution::database::addMii(testMii);

        fs::path exportPath = testTempDir / "miixports" / "createid_export.mii";
        REQUIRE(miivolution::storage::exportMii(testMii.createID, exportPath));
        REQUIRE(fs::exists(exportPath));
    }

    SECTION("Export non-existent CreateID fails") {
        RFLCreateID fakeID{};
        for (int i = 0; i < 8; i++) {
            fakeID.data[i] = 0xFF;
        }

        fs::path exportPath = testTempDir / "miixports" / "fake.mii";
        REQUIRE_FALSE(miivolution::storage::exportMii(fakeID, exportPath));
    }
}

TEST_CASE("Storage - Import Mii from file", "[storage][import]") {
    MiivolutionAPIFixture fixture;
    miivolution::database::loadDatabase();

    fs::create_directories(testTempDir / "miixports");

    SECTION("Can import Mii that was exported") {
        auto original = createTestMii("Import", 55, 66);

        fs::path exportPath = testTempDir / "test_import.mii";
        miivolution::storage::exportMii(original, exportPath);

        miivolution::mii::MII_DATA_STRUCT imported{};
        REQUIRE(miivolution::storage::importMii(exportPath, imported));

        REQUIRE(imported.height == 55);
        REQUIRE(imported.build == 66);
    }

    SECTION("Imported Mii matches original") {
        auto original = createTestMii("Match");
        original.hairType = 42;
        original.eyeColor = 3;

        fs::path exportPath = testTempDir / "match.mii";
        miivolution::storage::exportMii(original, exportPath);

        miivolution::mii::MII_DATA_STRUCT imported{};
        miivolution::storage::importMii(exportPath, imported);

        REQUIRE(imported.hairType == 42);
        REQUIRE(imported.eyeColor == 3);
    }

    SECTION("Import non-existent file fails") {
        miivolution::mii::MII_DATA_STRUCT mii{};
        fs::path fakePath = testTempDir / "nonexistent.mii";
        REQUIRE_FALSE(miivolution::storage::importMii(fakePath, mii));
    }
}

TEST_CASE("Storage - Import Mii directly to database", "[storage][import]") {
    MiivolutionAPIFixture fixture;
    miivolution::database::loadDatabase();
    clearDatabase();

    fs::create_directories(testTempDir / "miixports");

    SECTION("Can import Mii directly to DB") {
        auto original = createTestMii("DirectImport");

        fs::path exportPath = testTempDir / "test_direct.mii";
        miivolution::storage::exportMii(original, exportPath);

        u16 importedIndex = 0;
        REQUIRE(miivolution::storage::importMiiToDB(exportPath, &importedIndex));
        REQUIRE(miivolution::database::getMiiCount() == 1);
    }

    SECTION("Imported Mii is retrievable from DB") {
        auto original = createTestMii("Retrieve", 99, 11);

        fs::path exportPath = testTempDir / "retrieve.mii";
        miivolution::storage::exportMii(original, exportPath);

        u16 importedIndex = 0;
        miivolution::storage::importMiiToDB(exportPath, &importedIndex);

        miivolution::mii::MII_DATA_STRUCT fromDB{};
        REQUIRE(miivolution::database::getMii(importedIndex, fromDB));
        REQUIRE(fromDB.height == 99);
        REQUIRE(fromDB.build == 11);
    }

    SECTION("Import without output index works") {
        auto original = createTestMii("NoIndex");

        fs::path exportPath = testTempDir / "noindex.mii";
        miivolution::storage::exportMii(original, exportPath);

        REQUIRE(miivolution::storage::importMiiToDB(exportPath, nullptr));
        REQUIRE(miivolution::database::getMiiCount() == 1);
    }
}

TEST_CASE("Storage - Batch import", "[storage][import]") {
    MiivolutionAPIFixture fixture;
    miivolution::database::loadDatabase();
    clearDatabase();

    fs::path importsDir = testTempDir / "miimports";
    fs::create_directories(importsDir);

    SECTION("Can import multiple Miis from directory") {
        fs::remove_all(importsDir);
        fs::create_directories(importsDir);

        auto mii1 = createTestMii("Batch1");
        auto mii2 = createTestMii("Batch2");
        auto mii3 = createTestMii("Batch3");

        miivolution::storage::exportMii(mii1, importsDir / "batch1.mii");
        miivolution::storage::exportMii(mii2, importsDir / "batch2.mii");
        miivolution::storage::exportMii(mii3, importsDir / "batch3.mii");

        std::vector<u16> indices;
        u32 imported = miivolution::storage::importAllMiis(&indices);

        REQUIRE(imported == 3);
        REQUIRE(indices.size() == 3);
        REQUIRE(miivolution::database::getMiiCount() == 3);
    }

    SECTION("Import all without indices output works") {
        fs::remove_all(importsDir);
        fs::create_directories(importsDir);

        auto mii1 = createTestMii("NoIndices1");
        auto mii2 = createTestMii("NoIndices2");

        miivolution::storage::exportMii(mii1, importsDir / "ni1.mii");
        miivolution::storage::exportMii(mii2, importsDir / "ni2.mii");

        u32 imported = miivolution::storage::importAllMiis(nullptr);
        REQUIRE(imported == 2);
    }

    SECTION("Import all with empty directory returns zero") {
        fs::remove_all(importsDir);
        fs::create_directories(importsDir);

        u32 imported = miivolution::storage::importAllMiis();
        REQUIRE(imported == 0);
    }

    SECTION("Import all skips non-.mii files") {
        auto mii = createTestMii("OnlyMii");
        miivolution::storage::exportMii(mii, importsDir / "valid.mii");

        std::ofstream txtFile(importsDir / "ignore.txt");
        txtFile << "not a mii file";
        txtFile.close();

        u32 imported = miivolution::storage::importAllMiis();
        REQUIRE(imported == 1);
    }
}

TEST_CASE("Storage - Get imports and exports", "[storage]") {
    MiivolutionAPIFixture fixture;

    fs::path importsDir = testTempDir / "miimports";
    fs::path exportsDir = testTempDir / "miixports";
    fs::create_directories(importsDir);
    fs::create_directories(exportsDir);

    SECTION("getMiimports returns files in imports directory") {
        auto testMii = createTestMii("InImports");
        miivolution::storage::exportMii(testMii, importsDir / "import1.mii");
        miivolution::storage::exportMii(testMii, importsDir / "import2.mii");

        auto imports = miivolution::storage::getMiimports();
        REQUIRE(imports.size() >= 2);
    }

    SECTION("getMiixports returns files in exports directory") {
        auto testMii = createTestMii("InExports");
        miivolution::storage::exportMii(testMii, exportsDir / "export1.mii");
        miivolution::storage::exportMii(testMii, exportsDir / "export2.mii");

        auto exports = miivolution::storage::getMiixports();
        REQUIRE(exports.size() >= 2);
    }

    SECTION("Empty directories return empty lists") {
        fs::remove_all(importsDir);
        fs::remove_all(exportsDir);
        fs::create_directories(importsDir);
        fs::create_directories(exportsDir);

        auto imports = miivolution::storage::getMiimports();
        auto exports = miivolution::storage::getMiixports();

        REQUIRE(imports.empty());
        REQUIRE(exports.empty());
    }
}

TEST_CASE("Storage - Export and re-import roundtrip", "[storage][roundtrip]") {
    MiivolutionAPIFixture fixture;
    miivolution::database::loadDatabase();
    clearDatabase();

    fs::create_directories(testTempDir / "miixports");

    SECTION("Complete roundtrip preserves data") {
        auto original = createTestMii("Roundtrip", 123, 45);
        original.hairType = 67;
        original.eyeColor = 5;
        original.mouthType = 23;

        u16 originalIndex = 0;
        miivolution::database::addMii(original, &originalIndex);

        fs::path exportPath = testTempDir / "roundtrip.mii";
        miivolution::storage::exportMii(originalIndex, exportPath);

        miivolution::database::deleteMii(originalIndex);
        REQUIRE(miivolution::database::getMiiCount() == 0);

        u16 importedIndex = 0;
        miivolution::storage::importMiiToDB(exportPath, &importedIndex);

        miivolution::mii::MII_DATA_STRUCT final{};
        miivolution::database::getMii(importedIndex, final);

        REQUIRE(final.height == 123);
        REQUIRE(final.build == 45);
        REQUIRE(final.hairType == 67);
        REQUIRE(final.eyeColor == 5);
        REQUIRE(final.mouthType == 23);
    }
}

TEST_CASE("Storage - Cleanup verifies temp directory removal", "[storage][cleanup]") {
    {
        MiivolutionAPIFixture fixture;
        miivolution::database::loadDatabase();

        fs::create_directories(testTempDir / "miixports");

        auto testMii = createTestMii("Cleanup");
        fs::path exportPath = testTempDir / "miixports" / "cleanup.mii";
        miivolution::storage::exportMii(testMii, exportPath);

        REQUIRE(fs::exists(testTempDir));
        REQUIRE(fs::exists(exportPath));
    }

    REQUIRE_FALSE(fs::exists(testTempDir));
}

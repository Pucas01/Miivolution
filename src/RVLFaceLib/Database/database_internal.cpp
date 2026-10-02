#include "RVLFaceLib/RFLi_Database.h"
#include "RevoInternal/util.hpp"
#include "RevoInternal/endian.hpp"
#include <fstream>
#include <cstring>
#include <filesystem>

namespace {
    RFLiDatabase g_database;
    bool g_loaded = false;
}

static u16 calculateCRC16(const void* data, u32 size) {
    const u8* bytes = static_cast<const u8*>(data);
    u16 crc = 0;

    for (u32 i = 0; i < size; i++) {
        crc ^= static_cast<u16>(bytes[i]) << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

static bool loadDatabaseInternal() {
    const auto dbPath = miivolution::util::getFaceDatabase();

    if (!std::filesystem::exists(dbPath)) {
        return false;
    }

    std::ifstream file(dbPath, std::ios::binary);
    if (!file) {
        return false;
    }

    file.read(reinterpret_cast<char*>(&g_database), sizeof(RFLiDatabase));
    if (!file) {
        return false;
    }

    const u32 expectedRNOD = 0x524E4F44;
    const u32 expectedRNHD = 0x524E4844;

    if (revointernal::readBE32(reinterpret_cast<const u8*>(&g_database.identifier)) != expectedRNOD) {
        return false;
    }

    if (revointernal::readBE32(reinterpret_cast<const u8*>(&g_database.hidden.identifier)) != expectedRNHD) {
        return false;
    }

    u16 storedCRC;
    file.read(reinterpret_cast<char*>(&storedCRC), sizeof(u16));
    storedCRC = revointernal::readBE16(reinterpret_cast<const u8*>(&storedCRC));

    u16 calculatedCRC = calculateCRC16(&g_database, sizeof(RFLiDatabase));
    if (calculatedCRC != storedCRC) {
        return false;
    }

    g_loaded = true;
    return true;
}

static bool createEmptyDatabaseInternal() {
    std::memset(&g_database, 0, sizeof(RFLiDatabase));

    const u32 rnod = 0x524E4F44;
    const u32 rnhd = 0x524E4844;

    u8* idPtr = reinterpret_cast<u8*>(&g_database.identifier);
    revointernal::writeBE32(idPtr, rnod);

    u8* hiddenIdPtr = reinterpret_cast<u8*>(&g_database.hidden.identifier);
    revointernal::writeBE32(hiddenIdPtr, rnhd);

    g_database.hidden.head = -1;
    g_database.hidden.tail = -1;

    g_loaded = true;

    const auto dbPath = miivolution::util::getFaceDatabase();
    std::filesystem::create_directories(dbPath.parent_path());

    std::ofstream file(dbPath, std::ios::binary | std::ios::trunc);
    if (!file) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(&g_database), sizeof(RFLiDatabase));

    u16 crc = calculateCRC16(&g_database, sizeof(RFLiDatabase));
    u8 crcBytes[2];
    revointernal::writeBE16(crcBytes, crc);
    file.write(reinterpret_cast<const char*>(crcBytes), sizeof(u16));

    return file.good();
}

static bool saveDatabaseInternal() {
    if (!g_loaded) {
        return false;
    }

    const auto dbPath = miivolution::util::getFaceDatabase();
    std::filesystem::create_directories(dbPath.parent_path());

    std::ofstream file(dbPath, std::ios::binary | std::ios::trunc);
    if (!file) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(&g_database), sizeof(RFLiDatabase));

    u16 crc = calculateCRC16(&g_database, sizeof(RFLiDatabase));
    u8 crcBytes[2];
    revointernal::writeBE16(crcBytes, crc);
    file.write(reinterpret_cast<const char*>(crcBytes), sizeof(u16));

    return file.good();
}

extern "C" {

RFLiDatabase* RFLiGetDatabase() {
    if (!g_loaded) {
        if (!loadDatabaseInternal()) {
            createEmptyDatabaseInternal();
        }
    }
    return g_loaded ? &g_database : nullptr;
}

BOOL RFLiDBIsLoaded() {
    return g_loaded ? TRUE : FALSE;
}

void RFLiMarkDatabaseDirty() {
    if (g_loaded) {
        saveDatabaseInternal();
    }
}

}
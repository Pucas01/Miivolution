#include "RVLFaceLib/RFLi_Database.h"
#include "RevoInternal/util.hpp"
#include "RevoInternal/endian.hpp"
#include <fstream>
#include <cstring>
#include <filesystem>

#include "RevoInternal/log.hpp"

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
    LOG_DEBUG("[RFLi] Attempting to load database from: %s", dbPath.string().c_str());

    if (!std::filesystem::exists(dbPath)) {
        LOG_DEBUG("[RFLi] Database file does not exist at path");
        return false;
    }

    std::ifstream file(dbPath, std::ios::binary);
    if (!file) {
        LOG_DEBUG("[RFLi] Failed to open database file");
        return false;
    }

    file.read(reinterpret_cast<char*>(&g_database), sizeof(RFLiDatabase));
    if (!file) {
        LOG_DEBUG("[RFLi] Failed to read database file");
        return false;
    }

    const u32 expectedRNOD = 0x524E4F44;
    const u32 expectedRNHD = 0x524E4844;

    if (revointernal::readBE32(reinterpret_cast<const u8*>(&g_database.identifier)) != expectedRNOD) {
        LOG_DEBUG("[RFLi] Invalid RNOD magic in database");
        return false;
    }

    if (revointernal::readBE32(reinterpret_cast<const u8*>(&g_database.hidden.identifier)) != expectedRNHD) {
        LOG_DEBUG("[RFLi] Invalid RNHD magic in database");
        return false;
    }

    u16 storedCRC;
    file.read(reinterpret_cast<char*>(&storedCRC), sizeof(u16));
    storedCRC = revointernal::readBE16(reinterpret_cast<const u8*>(&storedCRC));

    u16 calculatedCRC = calculateCRC16(&g_database, sizeof(RFLiDatabase));
    if (calculatedCRC != storedCRC) {
        LOG_DEBUG("[RFLi] CRC mismatch - stored: 0x%04X, calculated: 0x%04X", storedCRC, calculatedCRC);
        return false;
    }

    LOG_DEBUG("[RFLi] Database loaded successfully");
    g_loaded = true;
    return true;
}

static bool createEmptyDatabaseInternal() {
    LOG_DEBUG("[RFLi] Creating empty database");
    std::memset(&g_database, 0, sizeof(RFLiDatabase));

    const u32 rnod = 0x524E4F44;
    const u32 rnhd = 0x524E4844;

    u8* idPtr = reinterpret_cast<u8*>(&g_database.identifier);
    revointernal::writeBE32(idPtr, rnod);

    u8* hiddenIdPtr = reinterpret_cast<u8*>(&g_database.hidden.identifier);
    revointernal::writeBE32(hiddenIdPtr, rnhd);

    // Initialize hidden database linked list
    g_database.hidden.head = -1;
    g_database.hidden.tail = -1;

    g_loaded = true;

    // Save the empty database
    const auto dbPath = miivolution::util::getFaceDatabase();
    LOG_DEBUG("[RFLi] Saving empty database to: %s", dbPath.string().c_str());
    std::filesystem::create_directories(dbPath.parent_path());

    std::ofstream file(dbPath, std::ios::binary | std::ios::trunc);
    if (!file) {
        LOG_DEBUG("[RFLi] Failed to create database file");
        return false;
    }

    file.write(reinterpret_cast<const char*>(&g_database), sizeof(RFLiDatabase));

    u16 crc = calculateCRC16(&g_database, sizeof(RFLiDatabase));
    u8 crcBytes[2];
    revointernal::writeBE16(crcBytes, crc);
    file.write(reinterpret_cast<const char*>(crcBytes), sizeof(u16));

    LOG_DEBUG("[RFLi] Empty database created successfully");
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
        LOG_DEBUG("[RFLi] Error saving database file: %s", dbPath.string().c_str());
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
    // Auto-load on first access
    if (!g_loaded) {
        LOG_DEBUG("[RFLi] Database not loaded, attempting to load or create");
        if (!loadDatabaseInternal()) {
            LOG_DEBUG("[RFLi] Load failed, creating empty database");
            createEmptyDatabaseInternal();
        }
    }
    return g_loaded ? &g_database : nullptr;
}

BOOL RFLiDBIsLoaded() {
    LOG_TRACE("[RFLi] Database loaded status: %d", g_loaded ? 1 : 0);
    return g_loaded ? TRUE : FALSE;
}

void RFLiMarkDatabaseDirty() {
    if (g_loaded) {
        LOG_DEBUG("[RFLi] Database marked dirty, saving");
        saveDatabaseInternal();
    }
}

}
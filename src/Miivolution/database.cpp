#include "Miivolution/database.hpp"
#include "RVLFaceLib/RFLi_Database.h"
#include "RevoInternal/util.hpp"
#include <cstring>

extern "C" {
    RFLiDatabase* RFLiGetDatabase();
    BOOL RFLiDBIsLoaded();
    void RFLiMarkDatabaseDirty();
}

namespace miivolution::database {

bool loadDatabase() {
    // Database is lazily loaded by RFLiGetDatabase()
    RFLiDatabase* db = RFLiGetDatabase();
    return db != nullptr;
}

bool saveDatabase() {
    // Explicit save (modifications auto-save already)
    RFLiMarkDatabaseDirty();
    return RFLiDBIsLoaded() == TRUE;
}

bool createEmptyDatabase() {
    return loadDatabase();
}

bool isLoaded() {
    return RFLiDBIsLoaded() == TRUE;
}

u32 getMiiCount() {
    RFLiDatabase* db = RFLiGetDatabase();
    if (!db) {
        return 0;
    }

    u32 count = 0;
    for (u32 i = 0; i < RFL_DB_CHAR_MAX; i++) {
        bool hasName = false;
        for (int j = 0; j < RFL_NAME_LEN; j++) {
            if (db->rawData[i].name[j] != 0) {
                hasName = true;
                break;
            }
        }

        if (hasName) {
            count++;
        }
    }

    return count;
}

bool getMii(u16 index, mii::MII_DATA_STRUCT& out) {
    RFLiDatabase* db = RFLiGetDatabase();
    if (!db || index >= RFL_DB_CHAR_MAX) {
        return false;
    }

    std::memcpy(&out, &db->rawData[index], sizeof(mii::MII_DATA_STRUCT));
    return true;
}

bool setMii(u16 index, const mii::MII_DATA_STRUCT& data) {
    RFLiDatabase* db = RFLiGetDatabase();
    if (!db || index >= RFL_DB_CHAR_MAX) {
        return false;
    }

    std::memcpy(&db->rawData[index], &data, sizeof(mii::MII_DATA_STRUCT));
    RFLiMarkDatabaseDirty();
    return true;
}

s32 findMiiByCreateID(const RFLCreateID& id) {
    RFLiDatabase* db = RFLiGetDatabase();
    if (!db) {
        return -1;
    }

    for (u32 i = 0; i < RFL_DB_CHAR_MAX; i++) {
        if (std::memcmp(&db->rawData[i].createID, &id, sizeof(RFLCreateID)) == 0) {
            return static_cast<s32>(i);
        }
    }

    return -1;
}

std::vector<u16> findMiisByName(const std::string& name) {
    std::vector<u16> indices;
    RFLiDatabase* db = RFLiGetDatabase();
    if (!db || name.empty()) {
        return indices;
    }

    u16 nameU16[RFL_NAME_LEN] = {0};
    size_t searchLen = 0;
    for (size_t i = 0; i < name.length() && i < RFL_NAME_LEN; i++) {
        nameU16[i] = static_cast<u16>(name[i]);
        searchLen++;
    }

    for (u32 i = 0; i < RFL_DB_CHAR_MAX; i++) {
        bool isEmpty = true;
        bool matches = true;

        for (int j = 0; j < RFL_NAME_LEN; j++) {
            if (db->rawData[i].name[j] != 0) {
                isEmpty = false;
            }
        }

        if (isEmpty) continue;

        for (size_t j = 0; j < searchLen; j++) {
            if (db->rawData[i].name[j] != nameU16[j]) {
                matches = false;
                break;
            }
        }

        for (size_t j = searchLen; j < RFL_NAME_LEN; j++) {
            if (db->rawData[i].name[j] != 0) {
                matches = false;
                break;
            }
        }

        if (matches) {
            indices.push_back(static_cast<u16>(i));
        }
    }

    return indices;
}

s32 findEmptySlot() {
    RFLiDatabase* db = RFLiGetDatabase();
    if (!db) {
        return -1;
    }

    for (u32 i = 0; i < RFL_DB_CHAR_MAX; i++) {
        bool isEmpty = true;
        for (int j = 0; j < RFL_NAME_LEN; j++) {
            if (db->rawData[i].name[j] != 0) {
                isEmpty = false;
                break;
            }
        }

        if (isEmpty) {
            return static_cast<s32>(i);
        }
    }

    return -1;
}

bool addMii(const mii::MII_DATA_STRUCT& data, u16* outIndex) {
    s32 slot = findEmptySlot();
    if (slot < 0) {
        return false;
    }

    if (!setMii(static_cast<u16>(slot), data)) {
        return false;
    }

    if (outIndex) {
        *outIndex = static_cast<u16>(slot);
    }

    return true;
}

bool deleteMii(u16 index) {
    RFLiDatabase* db = RFLiGetDatabase();
    if (!db || index >= RFL_DB_CHAR_MAX) {
        return false;
    }

    std::memset(&db->rawData[index], 0, sizeof(RFLiCharData));
    RFLiMarkDatabaseDirty();
    return true;
}

std::filesystem::path getDatabasePath() {
    return util::getFaceDatabase();
}

u16 calculateCRC16(const void* data, u32 size) {
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

}
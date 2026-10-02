#include "RFL_Database.h"
#include "RFL_System.h"
#include "RVLFaceLib/internal.hpp"
#include "RVLFaceLib/RFLi_Database.h"
#include "RevoInternal/log.hpp"
#include <cstring>

extern "C" {
    extern RFLiDatabase* RFLiGetDatabase();
    extern BOOL RFLiDBIsLoaded();
}

extern "C" {

BOOL RFLIsAvailableOfficialData(u16 index) {
    LOG_TRACE("[RFLi] RFLIsAvailableOfficialData called with index %d", index);

    if (!RFLAvailable()) {
        LOG_DEBUG("[RFLi] RFL not available");
        return FALSE;
    }

    if (index >= RFL_DB_CHAR_MAX) {
        LOG_DEBUG("[RFLi] Index %d out of range (max %d)", index, RFL_DB_CHAR_MAX);
        return FALSE;
    }

    // Get database (auto-loads if needed)
    RFLiDatabase* db = RFLiGetDatabase();
    if (!db) {
        LOG_DEBUG("[RFLi] Failed to get database");
        return FALSE;
    }

    // Check placeholder official index (default guest Mii)
    if (index == RFLiPlaceholderOfficialIndex) {
        LOG_TRACE("[RFLi] Index is placeholder, returning TRUE");
        return TRUE;
    }

    // Check if Mii has a valid name (not empty)
    for (int i = 0; i < RFL_NAME_LEN; i++) {
        if (db->rawData[index].name[i] != 0) {
            LOG_TRACE("[RFLi] Mii at index %d has valid name, returning TRUE", index);
            return TRUE;
        }
    }

    LOG_DEBUG("[RFLi] Mii at index %d has no name, returning FALSE", index);
    return FALSE;
}

BOOL RFLSearchOfficialData(const RFLCreateID* id, u16* index) {
    LOG_DEBUG("[RFLi] RFLSearchOfficialData called");

    if (!id || !index) {
        LOG_DEBUG("[RFLi] Invalid parameters (id=%p, index=%p)", (void*)id, (void*)index);
        return FALSE;
    }

    if (!RFLAvailable()) {
        LOG_DEBUG("[RFLi] RFL not available");
        return FALSE;
    }

    // Get database (auto-loads if needed)
    RFLiDatabase* db = RFLiGetDatabase();
    if (!db) {
        LOG_DEBUG("[RFLi] Failed to get database, returning placeholder");
        *index = RFLiPlaceholderOfficialIndex;
        return FALSE;
    }

    // Log CreateID being searched
    LOG_DEBUG("[RFLi] Searching for CreateID: %02X%02X%02X%02X%02X%02X",
        id->data[0], id->data[1], id->data[2], id->data[3], id->data[4], id->data[5]);

    // Search database for matching CreateID
    for (u32 i = 0; i < RFL_DB_CHAR_MAX; i++) {
        if (std::memcmp(&db->rawData[i].createID, id, sizeof(RFLCreateID)) == 0) {
            // Check if this slot actually has a Mii (has a name)
            bool hasName = false;
            for (int j = 0; j < RFL_NAME_LEN; j++) {
                if (db->rawData[i].name[j] != 0) {
                    hasName = true;
                    break;
                }
            }

            if (hasName) {
                LOG_DEBUG("[RFLi] Found matching Mii at index %d", i);
                *index = static_cast<u16>(i);
                return TRUE;
            }
        }
    }

    // Not found, so we return a placeholder
    LOG_DEBUG("[RFLi] CreateID not found in database, returning placeholder");
    *index = RFLiPlaceholderOfficialIndex;
    return FALSE;
}

}

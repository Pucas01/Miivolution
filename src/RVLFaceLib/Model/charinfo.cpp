#include "RFL_DataUtility.h"
#include "RFL_Model.h"
#include "RFL_System.h"
#include "RFL_Database.h"
#include "RVLFaceLib/RFLi_Types.h"
#include "RVLFaceLib/RFLi_Database.h"
#include "RevoInternal/log.hpp"

extern "C" {
    void RFLiConvertRaw2Info(const RFLiCharData* data, RFLiCharInfo* info);
    RFLiDatabase* RFLiGetDatabase();
}

extern "C" {

RFLErrcode RFLiPickupCharInfo(void* info, RFLDataSource source, RFLMiddleDB* db, u16 index) {
    LOG_DEBUG("[RFLi] RFLiPickupCharInfo called - source=%d, index=%d", source, index);

    if (!info) {
        LOG_DEBUG("[RFLi] Invalid info parameter");
        return RFLErrcode_WrongParam;
    }

    if (!RFLAvailable()) {
        LOG_DEBUG("[RFLi] RFL not available");
        return RFLErrcode_NotAvailable;
    }

    auto* charInfo = static_cast<RFLiCharInfo*>(info);
    RFLErrcode err = RFLErrcode_Success;

    switch (source) {
    case RFLDataSource_Official:
        LOG_DEBUG("[RFLi] Using Official data source with index %d", index);

        // Check if the database has a Mii at this index
        if (RFLIsAvailableOfficialData(index)) {
            LOG_DEBUG("[RFLi] Official data available at index %d", index);

            // Get the database and convert the raw data
            RFLiDatabase* database = RFLiGetDatabase();
            if (database) {
                LOG_DEBUG("[RFLi] Converting raw data from database slot %d", index);
                RFLiConvertRaw2Info(&database->rawData[index], charInfo);

                char nameStr[RFL_NAME_LEN + 1] = {0};
                for (int i = 0; i < RFL_NAME_LEN && charInfo->personal.name[i] != 0; i++) {
                    if (charInfo->personal.name[i] < 128) {
                        nameStr[i] = static_cast<char>(charInfo->personal.name[i]);
                    }
                }
                LOG_DEBUG("[RFLi] Loaded Mii: %s", nameStr);
            } else {
                LOG_DEBUG("[RFLi] Database is null, falling back to default");
                RFLiGetDefaultData(charInfo, index);
            }
        } else {
            LOG_DEBUG("[RFLi] No official data at index %d, using guest Mii", index);
            RFLiGetDefaultData(charInfo, RFLiPlaceholderOfficialIndex);
        }
        err = RFLErrcode_Success;
        break;
    case RFLDataSource_Controller1:
    case RFLDataSource_Controller2:
    case RFLDataSource_Controller3:
    case RFLDataSource_Controller4:
        LOG_DEBUG("[RFLi] Controller data source not supported");
        err = RFLErrcode_Broken;
        break;
    case RFLDataSource_Middle:
        LOG_DEBUG("[RFLi] Middle database not supported");
        err = RFLErrcode_Broken;
        break;
    case RFLDataSource_Default:
        LOG_DEBUG("[RFLi] Using default guest Mii at index %d", index);
        RFLiGetDefaultData(charInfo, index);
        err = RFLErrcode_Success;
        break;
    default:
        LOG_DEBUG("[RFLi] Unknown data source: %d", source);
        err = RFLErrcode_WrongParam;
        break;
    }

    return err;
}

}

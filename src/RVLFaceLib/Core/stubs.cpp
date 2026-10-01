#include "RFL_Controller.h"
#include "RFL_Database.h"
#include "RFL_DataUtility.h"
#include "RFL_Icon.h"
#include "RFL_MiddleDatabase.h"
#include "RFL_Model.h"
#include "RFL_NANDLoader.h"
#include "RFL_NWC24.h"
#include "RVLFaceLib/internal.hpp"
#include "RFLi_Types.hpp"
#include <cstring>

extern "C" {

// =============================================================================
// RFL_DataUtility.h
// =============================================================================

RFLErrcode RFLGetAdditionalInfo(RFLAdditionalInfo* info, RFLDataSource source,
                                RFLMiddleDB* db, u16 index) {
    if (!info) {
        return RFLErrcode_WrongParam;
    }

    RFLiCharInfo charInfo;
    RFLErrcode err = RFLiPickupCharInfo(&charInfo, source, db, index);
    if (err != RFLErrcode_Success) {
        return err;
    }

    std::memset(info, 0, sizeof(*info));
    for (int i = 0; i < RFL_NAME_LEN; i++) {
        info->name[i] = charInfo.personal.name[i];
    }
    for (int i = 0; i < RFL_CREATOR_LEN; i++) {
        info->creator[i] = charInfo.personal.creator[i];
    }
    info->createID = charInfo.createID;
    info->sex = charInfo.personal.sex;
    info->bmonth = charInfo.personal.bmonth;
    info->bday = charInfo.personal.bday;
    info->color = charInfo.personal.color;
    info->favorite = charInfo.personal.favorite;
    info->height = charInfo.body.height;
    info->build = charInfo.body.build;
    info->skinColor = GXColor{0xF5, 0xC8, 0xA0, 0xFF};
    return RFLErrcode_Success;
}

// =============================================================================
// RFL_Icon.h
// =============================================================================

RFLErrcode RFLMakeIcon(void* buf, RFLDataSource source, RFLMiddleDB* middleDB,
                       u16 index, RFLExpression expression,
                       const RFLIconSetting* setting) {
    RVL_NOT_IMPL("RFLMakeIcon");
    return RFLErrcode_NotAvailable;
}

void RFLSetIconDrawDoneCallback(RFLCallback callback) {
    RVL_NOT_IMPL("RFLSetIconDrawDoneCallback");
}

} // extern "C"

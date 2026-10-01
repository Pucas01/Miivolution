#include "RVLFaceLib/internal.hpp"
#include "RVLFaceLib/RFL_Controller.h"
#include "RVLFaceLib/RFL_Database.h"
#include "RVLFaceLib/RFL_DataUtility.h"
#include "RVLFaceLib/RFL_Icon.h"
#include "RVLFaceLib/RFL_MiddleDatabase.h"
#include "RVLFaceLib/RFL_Model.h"
#include "RVLFaceLib/RFL_NANDLoader.h"
#include "RVLFaceLib/RFL_NWC24.h"

extern "C" {

// =============================================================================
// RFL_DataUtility.h
// =============================================================================

RFLErrcode RFLGetAdditionalInfo(RFLAdditionalInfo* info, RFLDataSource source,
                                RFLMiddleDB* db, u16 index) {
    RVL_NOT_IMPL("RFLGetAdditionalInfo");
    return RFLErrcode_NotAvailable;
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

// =============================================================================
// RFL_NWC24.h
// =============================================================================

RFLErrcode RFLCommitNWC24Msg(struct NWC24MsgObj* msg, u16 index) {
    RVL_NOT_IMPL("RFLCommitNWC24Msg");
    return RFLErrcode_NotAvailable;
}

} // extern "C"

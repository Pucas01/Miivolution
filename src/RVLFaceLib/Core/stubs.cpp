#include "RFL_Controller.h"
#include "RFL_Database.h"
#include "RFL_DataUtility.h"
#include "RFL_Icon.h"
#include "RFL_MiddleDatabase.h"
#include "RFL_Model.h"
#include "RFL_NANDLoader.h"
#include "RFL_NWC24.h"
#include "RVLFaceLib/internal.hpp"

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

} // extern "C"

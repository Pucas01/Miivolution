#include "RFL_DataUtility.h"
#include "RFL_Model.h"
#include "RFL_System.h"
#include "RFLi_Types.hpp"

extern "C" {

RFLErrcode RFLiPickupCharInfo(void* info, RFLDataSource source, RFLMiddleDB* db, u16 index) {
    if (!info) {
        return RFLErrcode_WrongParam;
    }

    if (!RFLAvailable()) {
        return RFLErrcode_NotAvailable;
    }

    auto* charInfo = static_cast<RFLiCharInfo*>(info);
    RFLErrcode err = RFLErrcode_Success;

    switch (source) {
    case RFLDataSource_Official:
        RFLiGetDefaultData(charInfo, RFLiPlaceholderOfficialIndex);
        err = RFLErrcode_Success;
        break;
    case RFLDataSource_Controller1:
    case RFLDataSource_Controller2:
    case RFLDataSource_Controller3:
    case RFLDataSource_Controller4:
        err = RFLErrcode_Broken;
        break;
    case RFLDataSource_Middle:
        err = RFLErrcode_Broken;
        break;
    case RFLDataSource_Default:
        RFLiGetDefaultData(charInfo, index);
        err = RFLErrcode_Success;
        break;
    default:
        err = RFLErrcode_WrongParam;
        break;
    }

    return err;
}

}

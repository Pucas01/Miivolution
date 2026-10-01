#include "RFL_Controller.h"
#include "RFL_System.h"
#include "RVLFaceLib/internal.hpp"

extern "C" {

RFLErrcode RFLLoadControllerAsync(s32 chan) {
    if (!RFLAvailable()) {
        return RFLErrcode_NotAvailable;
    }

    if (chan < 0 || chan >= 4) {
        return RFLErrcode_WrongParam;
    }

    return RFLErrcode_NotAvailable;
}

BOOL RFLIsAvailableControllerData(s32 chan, u16 index) {
    if (!RFLAvailable()) {
        return FALSE;
    }

    if (chan < 0 || chan >= 4) {
        return FALSE;
    }

    return FALSE;
}

}

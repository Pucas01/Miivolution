#include "RVLFaceLib/internal.hpp"
#include "RVLFaceLib/RFL_Database.h"
#include "RVLFaceLib/RFL_System.h"

extern "C" {

BOOL RFLIsAvailableOfficialData(u16 index) {
    if (!RFLAvailable()) {
        return FALSE;
    }

    return FALSE;
}

BOOL RFLSearchOfficialData(const RFLCreateID* id, u16* index) {
    if (!id || !index) {
        return FALSE;
    }

    if (!RFLAvailable()) {
        return FALSE;
    }

    return FALSE;
}

}

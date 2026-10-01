#include "RFL_Database.h"
#include "RFL_System.h"
#include "RVLFaceLib/internal.hpp"

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

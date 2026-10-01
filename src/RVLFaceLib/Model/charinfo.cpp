#include "RVLFaceLib/internal.hpp"
#include "RVLFaceLib/RFL_DataUtility.h"
#include "RVLFaceLib/RFL_Model.h"
#include "RVLFaceLib/RFL_System.h"
#include <cstring>

namespace {

struct RFLiCharInfo {
    struct { u16 rawdata; } faceline;
    struct { u16 rawdata; } hair;
    struct { u32 rawdata; } eye;
    struct { u32 rawdata; } eyebrow;
    struct { u16 rawdata; } nose;
    struct { u16 rawdata; } mouth;
    struct { u16 rawdata; } beard;
    struct { u16 rawdata; } glass;
    struct { u16 rawdata; } mole;
    struct { u8 height; u8 build; } personal;
    RFLCreateID createID;
    u16 sex;
    u16 favoriteColor;
    wchar_t name[10];
};

}

extern "C" {

RFLErrcode RFLiPickupCharInfo(void* info, RFLDataSource source, RFLMiddleDB* db, u16 index) {
    if (!info) {
        return RFLErrcode_WrongParam;
    }

    if (!RFLAvailable()) {
        return RFLErrcode_NotAvailable;
    }

    auto* charInfo = static_cast<RFLiCharInfo*>(info);
    std::memset(charInfo, 0, sizeof(RFLiCharInfo));

    charInfo->faceline.rawdata = 0;
    charInfo->hair.rawdata = 0;
    charInfo->eye.rawdata = 0;
    charInfo->eyebrow.rawdata = 0;
    charInfo->nose.rawdata = 0;
    charInfo->mouth.rawdata = 0;
    charInfo->beard.rawdata = 0;
    charInfo->glass.rawdata = 0;
    charInfo->mole.rawdata = 0;
    charInfo->personal.height = 64;
    charInfo->personal.build = 64;
    charInfo->sex = 0;
    charInfo->favoriteColor = 0;

    for (int i = 0; i < 8; i++) {
        charInfo->createID.data[i] = i;
    }

    wcscpy(charInfo->name, L"Mii");

    return RFLErrcode_Success;
}

void RFLiInitCharModel(RFLCharModel* model, void* info, void* work, RFLResolution res, u32 exprFlags) {
    // This would normally build the full model resources, but for now we just initialize the basics
    // The model was already set up in RFLInitCharModel
}

}

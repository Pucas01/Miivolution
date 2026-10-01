#include "internal.hpp"
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
// RFL_Database.h
// =============================================================================

BOOL RFLIsAvailableOfficialData(u16 index) {
    RVL_NOT_IMPL("RFLIsAvailableOfficialData");
    return FALSE;
}

BOOL RFLSearchOfficialData(const RFLCreateID* id, u16* index) {
    RVL_NOT_IMPL("RFLSearchOfficialData");
    return FALSE;
}

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
// RFL_MiddleDatabase.h
// =============================================================================

u32 RFLGetMiddleDBBufferSize(u16 size) {
    RVL_NOT_IMPL("RFLGetMiddleDBBufferSize");
    return 0;
}

void RFLInitMiddleDB(RFLMiddleDB* db, RFLMiddleDBType type, void* buffer,
                     u16 size) {
    RVL_NOT_IMPL("RFLInitMiddleDB");
}

// =============================================================================
// RFL_Model.h
// =============================================================================

void RFLSetCoordinate(RFLCoordinateType t1, RFLCoordinateType t2) {
    RVL_NOT_IMPL("RFLSetCoordinate");
}

u32 RFLGetModelBufferSize(RFLResolution res, u32 exprFlags) {
    RVL_NOT_IMPL("RFLGetModelBufferSize");
    return 0;
}

RFLErrcode RFLInitCharModel(RFLCharModel* model, RFLDataSource src,
                            RFLMiddleDB* db, u16 id, void* work,
                            RFLResolution res, u32 exprFlags) {
    RVL_NOT_IMPL("RFLInitCharModel");
    return RFLErrcode_NotAvailable;
}

void RFLSetMtx(RFLCharModel* model, const Mtx mvMtx) {
    RVL_NOT_IMPL("RFLSetMtx");
}

void RFLSetExpression(RFLCharModel* model, RFLExpression expr) {
    RVL_NOT_IMPL("RFLSetExpression");
}

RFLExpression RFLGetExpression(const RFLCharModel* model) {
    RVL_NOT_IMPL("RFLGetExpression");
    return RFLExp_Normal;
}

GXColor RFLGetFavoriteColor(RFLFavoriteColor color) {
    RVL_NOT_IMPL("RFLGetFavoriteColor");
    GXColor c = {0, 0, 0, 255};
    return c;
}

void RFLLoadDrawSetting(const RFLDrawSetting* setting) {
    RVL_NOT_IMPL("RFLLoadDrawSetting");
}

void RFLDrawOpa(const RFLCharModel* model) {
    RVL_NOT_IMPL("RFLDrawOpa");
}

void RFLDrawXlu(const RFLCharModel* model) {
    RVL_NOT_IMPL("RFLDrawXlu");
}

void RFLLoadVertexSetting(const RFLDrawCoreSetting* setting) {
    RVL_NOT_IMPL("RFLLoadVertexSetting");
}

void RFLLoadMaterialSetting(const RFLDrawCoreSetting* setting) {
    RVL_NOT_IMPL("RFLLoadMaterialSetting");
}

void RFLDrawOpaCore(const RFLCharModel* model,
                    const RFLDrawCoreSetting* setting) {
    RVL_NOT_IMPL("RFLDrawOpaCore");
}

void RFLDrawXluCore(const RFLCharModel* model,
                    const RFLDrawCoreSetting* setting) {
    RVL_NOT_IMPL("RFLDrawXluCore");
}

void RFLDrawShape(const RFLCharModel* model) {
    RVL_NOT_IMPL("RFLDrawShape");
}

// =============================================================================
// RFL_NANDLoader.h
// =============================================================================

BOOL RFLFreeCachedResource(void) {
    RVL_NOT_IMPL("RFLFreeCachedResource");
    return TRUE;
}

BOOL RFLIsResourceCached(void) {
    RVL_NOT_IMPL("RFLIsResourceCached");
    return FALSE;
}

// =============================================================================
// RFL_NWC24.h
// =============================================================================

RFLErrcode RFLCommitNWC24Msg(struct NWC24MsgObj* msg, u16 index) {
    RVL_NOT_IMPL("RFLCommitNWC24Msg");
    return RFLErrcode_NotAvailable;
}

// =============================================================================
// RFL_Controller.h
// =============================================================================

RFLErrcode RFLLoadControllerAsync(s32 chan) {
    RVL_NOT_IMPL("RFLLoadControllerAsync");
    return RFLErrcode_NotAvailable;
}

BOOL RFLIsAvailableControllerData(s32 chan, u16 index) {
    RVL_NOT_IMPL("RFLIsAvailableControllerData");
    return FALSE;
}

} // extern "C"

#include "RVLFaceLib/internal.hpp"
#include "RVLFaceLib/RFL_Model.h"
#include "RVLFaceLib/RFL_System.h"

#if DOLPHIN_INCLUDES
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#else
#include <revolution/gx.h>
#include <revolution/mtx.h>
#endif

#include <array>
#include <algorithm>
#include <bit>
#include <cstring>
#include <cstdint>

namespace {

struct CoordinateData {
    u8 uOff = 1;
    u8 fOff = 2;
    u8 rOff = 0;
    bool uRev = false;
    bool fRev = false;
    bool rRev = false;
};

constexpr u32 roundUp(u32 value, u32 alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

constexpr u32 getMaskSize(u32 resolution) {
    return 2 * (resolution * resolution);
}

constexpr u32 getMaskBufSize(RFLResolution resolution) {
    u32 size = 0;
    u32 res = static_cast<u32>(resolution);

    if (res & 32) size += getMaskSize(32);
    if (res & 64) size += getMaskSize(64);
    if (res & 128) size += getMaskSize(128);
    if (res & 256) size += getMaskSize(256);

    return size;
}

constexpr u32 countExpressions(u32 exprFlags) {
    return std::popcount(exprFlags);
}

constexpr std::array<GXColor, RFLFavoriteColor_Max> favoriteColors = {{
    {184, 64,  48,  255},
    {240, 120, 40,  255},
    {248, 216, 32,  255},
    {128, 200, 40,  255},
    {0,   116, 40,  255},
    {32,  72,  152, 255},
    {64,  160, 216, 255},
    {232, 96,  120, 255},
    {112, 44,  168, 255},
    {72,  56,  24,  255},
    {224, 224, 224, 255},
    {24,  24,  20,  255}
}};

constexpr GXColor white = {255, 255, 255, 255};

constexpr RFLDrawCoreSetting defaultDrawCoreSetting2Tev = {
    1, GX_TEXCOORD0, GX_TEXMAP0, 2, GX_TEV_SWAP0, GX_KCOLOR0, GX_TEVPREV, GX_PNMTX0, FALSE
};

constexpr RFLDrawCoreSetting defaultDrawCoreSetting1Tev = {
    1, GX_TEXCOORD0, GX_TEXMAP0, 1, GX_TEV_SWAP0, GX_KCOLOR0, GX_TEVPREV, GX_PNMTX0, FALSE
};

struct CharModelRes {
    u8 noseDl[0xC0];
    u8 capDl[0x560];
    u8 faceDl[0x2E0];
    u8 beardDl[0x160];
    u8 noselineDl[0x60];
    u8 maskDl[0x380];
    u8 glassesDl[0x40];

    u8 faceTex[0x4000];
    u8 capTex[0x400];
    u8 noseTex[0x400];
    u8 glassesTex[0x1000];

    s16 noseVtxPos[23 * 3];
    s16 noseVtxNrm[23 * 3];
    s16 capVtxPos[173 * 3];
    s16 capVtxNrm[246 * 3];
    s16 capVtxTxc[95 * 2];
    s16 faceVtxPos[66 * 3];
    s16 faceVtxNrm[66 * 3];
    s16 faceVtxTxc[115 * 2];
    s16 beardVtxPos[40 * 3];
    s16 beardVtxNrm[68 * 3];
    s16 noselineVtxPos[6 * 3];
    s16 noselineVtxNrm[2 * 3];
    s16 noselineVtxTxc[7 * 2];
    s16 maskVtxPos[88 * 3];
    s16 maskVtxNrm[86 * 3];
    s16 maskVtxTxc[176 * 2];
    s16 glassesVtxPos[4 * 3];
    s16 glassesVtxNrm[1 * 3];
    s16 glassesVtxTxc[4 * 2];

    GXTexObj faceTexObj;
    GXTexObj capTexObj;
    GXTexObj noseTexObj;
    GXTexObj glassesTexObj;

    s16* hairVtxPos;
    s16* hairVtxNrm;
    u8* hairDl;
    s16* foreheadVtxPos;
    s16* foreheadVtxNrm;
    u8* foreheadDl;

    u16 noseDlSize;
    u16 faceDlSize;
    u16 hairDlSize;
    u16 capDlSize;
    u16 foreheadDlSize;
    u16 beardDlSize;
    u16 noselineDlSize;
    u16 maskDlSize;
    u16 glassesDlSize;

    u8 facelineColor;
    u8 hairColor;
    u8 beardColor;
    u8 glassesColor;
    u8 favoriteColor;

    bool flipHair;
};

struct CharModelInternal {
    Mtx posMtx;
    Mtx nrmMtx;
    RFLExpression currentExpression = RFLExp_Normal;
    RFLResolution resolution = RFLResolution_128;
    CharModelRes* res = nullptr;
    GXTexObj* maskTexObj[RFLExp_Max] = {nullptr};
};

CoordinateData coordinateData;

}

extern "C" {

void RFLSetCoordinate(RFLCoordinateType u, RFLCoordinateType f) {

    union CoordBytes {
        RFLCoordinateType c;
        u8 b[4];
    };

    CoordBytes uu{.c = u};
    CoordBytes uf{.c = f};
    CoordBytes ur;

    ur.b[0] = (uu.b[1] * uf.b[2]) - (uu.b[2] * uf.b[1]);
    ur.b[1] = (uu.b[2] * uf.b[0]) - (uu.b[0] * uf.b[2]);
    ur.b[2] = (uu.b[0] * uf.b[1]) - (uu.b[1] * uf.b[0]);

    RFLCoordinateType r = ur.c;

    if (u & RFLCoordinateType_X) {
        coordinateData.uOff = 0;
    } else if (u & RFLCoordinateType_Y) {
        coordinateData.uOff = 1;
    } else {
        coordinateData.uOff = 2;
    }

    if (f & RFLCoordinateType_X) {
        coordinateData.fOff = 0;
    } else if (f & RFLCoordinateType_Y) {
        coordinateData.fOff = 1;
    } else {
        coordinateData.fOff = 2;
    }

    if (r & RFLCoordinateType_X) {
        coordinateData.rOff = 0;
    } else if (r & RFLCoordinateType_Y) {
        coordinateData.rOff = 1;
    } else {
        coordinateData.rOff = 2;
    }

    coordinateData.uRev = (u & RFLCoordinateType_RevMask) != 0;
    coordinateData.fRev = (f & RFLCoordinateType_RevMask) != 0;
    coordinateData.rRev = (r & RFLCoordinateType_RevMask) != 0;
}

u32 RFLGetModelBufferSize(RFLResolution res, u32 exprFlags) {

    constexpr u32 GX_TEXOBJ_SIZE = 32;

    const u32 exprNum = countExpressions(exprFlags);
    const u32 texSize = getMaskBufSize(res);

    return roundUp(sizeof(CharModelInternal), 32) +
           roundUp(exprNum * GX_TEXOBJ_SIZE, 32) +
           roundUp(sizeof(CharModelRes), 32) +
           roundUp(texSize * exprNum, 32);
}

extern "C" RFLErrcode RFLiPickupCharInfo(void* info, RFLDataSource source, RFLMiddleDB* db, u16 index);
extern "C" void RFLiInitCharModel(RFLCharModel* model, void* info, void* work, RFLResolution res, u32 exprFlags);

RFLErrcode RFLInitCharModel(RFLCharModel* model, RFLDataSource src,
                            RFLMiddleDB* db, u16 id, void* work,
                            RFLResolution res, u32 exprFlags) {
    if (!model || !work) {
        return RFLErrcode_WrongParam;
    }

    if (!RFLAvailable()) {
        return RFLErrcode_NotAvailable;
    }

    u8 charInfoBuf[96];
    RFLErrcode err = RFLiPickupCharInfo(charInfoBuf, src, db, id);
    if (err != RFLErrcode_Success) {
        return err;
    }

    std::memset(model, 0, sizeof(RFLCharModel));

    u8* workPtr = static_cast<u8*>(work);
    auto* internal = reinterpret_cast<CharModelInternal*>(workPtr);
    workPtr += roundUp(sizeof(CharModelInternal), 32);

    *reinterpret_cast<CharModelInternal**>(model) = internal;

    std::memset(internal, 0, sizeof(CharModelInternal));
    internal->currentExpression = RFLExp_Normal;
    internal->resolution = res;

    const u32 exprNum = countExpressions(exprFlags);
    auto* exprTexObj = reinterpret_cast<GXTexObj*>(workPtr);
    workPtr += roundUp(exprNum * sizeof(GXTexObj), 32);

    for (u32 i = 0; i < RFLExp_Max; i++) {
        if (exprFlags & (1 << i)) {
            internal->maskTexObj[i] = exprTexObj;
            exprTexObj++;
        } else {
            internal->maskTexObj[i] = nullptr;
        }
    }

    internal->res = reinterpret_cast<CharModelRes*>(workPtr);
    std::memset(internal->res, 0, sizeof(CharModelRes));

    RFLiInitCharModel(model, charInfoBuf, work, res, exprFlags);

    return RFLErrcode_Success;
}

void RFLSetMtx(RFLCharModel* model, const Mtx mvMtx) {
    if (!model || !mvMtx) return;

    auto* internal = *reinterpret_cast<CharModelInternal**>(model);
    if (!internal) return;

    MTXCopy(mvMtx, internal->posMtx);
    MTXInvXpose(mvMtx, internal->nrmMtx);
}

void RFLSetExpression(RFLCharModel* model, RFLExpression expr) {
    if (!model) return;

    auto* internal = *reinterpret_cast<CharModelInternal**>(model);
    if (!internal) return;

    internal->currentExpression = expr;
}

RFLExpression RFLGetExpression(const RFLCharModel* model) {
    if (!model) return RFLExp_Normal;

    auto* internal = *reinterpret_cast<CharModelInternal* const*>(model);
    if (!internal) return RFLExp_Normal;

    return internal->currentExpression;
}

GXColor RFLGetFavoriteColor(RFLFavoriteColor color) {
    if (color >= RFLFavoriteColor_Max) {
        return favoriteColors[0];
    }

    return favoriteColors[color];
}

void RFLLoadDrawSetting(const RFLDrawSetting* setting) {

    if (!setting) return;

    GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_OR, GX_NEVER, 0);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
    GXSetZMode(TRUE, GX_LEQUAL, TRUE);
    GXSetZCompLoc(setting->compLoc);
    GXSetColorUpdate(TRUE);
    GXSetAlphaUpdate(TRUE);
    GXSetDither(FALSE);
    GXSetDstAlpha(FALSE, 0);

    if (setting->lightEnable) {
        GXSetTevDirect(GX_TEVSTAGE1);
        GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
        GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);
        GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, GX_TEVPREV);

        RFLLoadMaterialSetting(&defaultDrawCoreSetting2Tev);
        RFLLoadVertexSetting(&defaultDrawCoreSetting2Tev);
        GXSetNumChans(1);

        GXSetChanCtrl(GX_COLOR0, TRUE, GX_SRC_REG, GX_SRC_REG, setting->lightMask, setting->diffuse, setting->attn);
        GXSetChanCtrl(GX_ALPHA0, FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
        GXSetChanAmbColor(GX_COLOR0, setting->ambColor);
        GXSetChanMatColor(GX_COLOR0, white);
    } else {
        RFLLoadMaterialSetting(&defaultDrawCoreSetting1Tev);
        RFLLoadVertexSetting(&defaultDrawCoreSetting1Tev);
        GXSetNumChans(0);
    }
}

void RFLDrawOpa(const RFLCharModel* model) {
    RFLDrawOpaCore(model, &defaultDrawCoreSetting2Tev);
}

void RFLDrawXlu(const RFLCharModel* model) {
    RFLDrawXluCore(model, &defaultDrawCoreSetting2Tev);
}

void RFLLoadVertexSetting(const RFLDrawCoreSetting* setting) {
    if (!setting) return;

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
    GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S16, 14);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 13);
    GXSetNumTexGens(setting->txcGenNum);
}

void RFLLoadMaterialSetting(const RFLDrawCoreSetting* setting) {
    if (!setting) return;

    GXSetTevSwapModeTable(setting->tevSwapTable, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetTevSwapModeTable(static_cast<GXTevSwapSel>(setting->tevSwapTable + 1), GX_CH_RED, GX_CH_ALPHA, GX_CH_BLUE, GX_CH_GREEN);
    GXSetNumTevStages(setting->tevStageNum);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, setting->tevOutRegID);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, 1, setting->tevOutRegID);
    GXSetTevKColorSel(GX_TEVSTAGE0, static_cast<GXTevKColorSel>(setting->tevKColorID + GX_TEV_KCSEL_K0));
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_1);
}

void RFLDrawOpaCore(const RFLCharModel* model, const RFLDrawCoreSetting* setting) {
    if (!model || !setting) return;

    auto* internal = *reinterpret_cast<CharModelInternal* const*>(model);

    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
    GXSetTevSwapMode(GX_TEVSTAGE0, setting->tevSwapTable, setting->tevSwapTable);
    GXSetCullMode(setting->reverseCulling ? GX_CULL_FRONT : GX_CULL_BACK);

    GXLoadPosMtxImm(internal->posMtx, setting->posNrmMtxID);
    GXLoadNrmMtxImm(internal->nrmMtx, setting->posNrmMtxID);
    GXSetCurrentMtx(setting->posNrmMtxID);

    GXSetTexCoordGen2(setting->txcID, GX_TG_MTX2x4, GX_TG_POS, 60, FALSE, 0x7D);
    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_KONST);
}

void RFLDrawXluCore(const RFLCharModel* model, const RFLDrawCoreSetting* setting) {
    if (!model || !setting) return;

    auto* internal = *reinterpret_cast<CharModelInternal* const*>(model);

    GXSetTevOrder(GX_TEVSTAGE0, setting->txcID, setting->texMapID, GX_COLOR_NULL);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(GX_TEVSTAGE0, setting->tevSwapTable, setting->tevSwapTable);

    GXLoadPosMtxImm(internal->posMtx, setting->posNrmMtxID);
    GXLoadNrmMtxImm(internal->nrmMtx, setting->posNrmMtxID);
    GXSetCurrentMtx(setting->posNrmMtxID);

    GXSetTexCoordGen2(setting->txcID, GX_TG_MTX2x4, GX_TG_TEX0, 60, FALSE, 0x7D);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetCullMode(setting->reverseCulling ? GX_CULL_FRONT : GX_CULL_BACK);
}

void RFLDrawShape(const RFLCharModel* model) {
    if (!model) return;

    auto* internal = *reinterpret_cast<CharModelInternal* const*>(model);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
    GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S16, 14);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 13);

    GXLoadPosMtxImm(internal->posMtx, GX_PNMTX0);
    GXLoadNrmMtxImm(internal->nrmMtx, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
}

}

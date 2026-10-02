#include "RFL_Model.h"
#include "RFL_System.h"

#include "RVLFaceLib/internal.hpp"
#include "model_internal.hpp"
#include "RVLFaceLib/RFLi_Types.h"
#include "RVLFaceLib/resource.hpp"
#include <cmath>
#include <vector>
#include "RevoInternal/endian.hpp"

using namespace rvlfacelib;

extern "C" {
    void* RFLiAlloc32(u32 size);
    void RFLiFree(void* block);
    u32 RFLiGetShapeSize(u32 part, u16 file);
    void RFLiLoadShape(u32 part, u16 file, void* dest);
    u32 RFLiGetShpTexSize(u32 part, u16 file);
    void* RFLiLoadShpTexture(u32 part, u16 file, void* dest);
}

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

constexpr std::array<GXColor, 6> facelineColors = {{
    {240, 216, 196, 255},
    {255, 188, 128, 255},
    {216, 136, 80,  255},
    {255, 176, 144, 255},
    {152, 80,  48,  255},
    {82,  46,  28,  255}
}};

constexpr std::array<GXColor, 8> hairColors = {{
    {30,  26,  24,  255},
    {56,  32,  21,  255},
    {85,  38,  23,  255},
    {112, 64,  36,  255},
    {114, 114, 120, 255},
    {73,  54,  26,  255},
    {122, 89,  40,  255},
    {193, 159, 100, 255}
}};

constexpr std::array<GXColor, 8> beardColors = {{
    {30,  26,  24,  255},
    {56,  32,  21,  255},
    {85,  38,  23,  255},
    {112, 64,  36,  255},
    {114, 114, 120, 255},
    {73,  54,  26,  255},
    {122, 89,  40,  255},
    {193, 159, 100, 255}
}};

constexpr std::array<GXColor, 6> glassColors = {{
    {16,  16, 16, 255},
    {96,  56, 16, 255},
    {152, 24, 16, 255},
    {32,  48, 96, 255},
    {144, 88, 0,  255},
    {96,  88, 80, 255}
}};

constexpr RFLDrawCoreSetting defaultDrawCoreSetting2Tev = {
    1, GX_TEXCOORD0, GX_TEXMAP0, 2, GX_TEV_SWAP0, GX_KCOLOR0, GX_TEVPREV, GX_PNMTX0, FALSE
};

constexpr RFLDrawCoreSetting defaultDrawCoreSetting1Tev = {
    1, GX_TEXCOORD0, GX_TEXMAP0, 1, GX_TEV_SWAP0, GX_KCOLOR0, GX_TEVPREV, GX_PNMTX0, FALSE
};

CoordinateData coordinateData = {
    .uOff = 1,
    .fOff = 2,
    .rOff = 0,
    .uRev = false,
    .fRev = false,
    .rRev = false
};

void transformCoordinate(s16* to, const s16* from) {
    to[coordinateData.rOff] = coordinateData.rRev ? -from[0] : from[0];
    to[coordinateData.uOff] = coordinateData.uRev ? -from[1] : from[1];
    to[coordinateData.fOff] = coordinateData.fRev ? -from[2] : from[2];
}

}

GXColor getFacelineColor(u8 index) {
    if (index >= facelineColors.size()) {
        return facelineColors[0];
    }
    return facelineColors[index];
}

GXColor getHairColor(u8 index) {
    if (index >= hairColors.size()) {
        return hairColors[0];
    }
    return hairColors[index];
}

GXColor getBeardColor(u8 index) {
    if (index >= beardColors.size()) {
        return beardColors[0];
    }
    return beardColors[index];
}

GXColor getGlassColor(u8 index) {
    if (index >= glassColors.size()) {
        return glassColors[0];
    }
    return glassColors[index];
}

extern "C" {

GXColor RFLiGetFacelineColor(u8 index) {
    return getFacelineColor(index);
}

GXColor RFLiGetHairColor(u8 index) {
    return getHairColor(index);
}

GXColor RFLiGetBeardColor(u8 index) {
    return getBeardColor(index);
}

GXColor RFLiGetGlassColor(u8 index) {
    return getGlassColor(index);
}

void RFLiTransformCoordinate(s16* to, const s16* from) {
    transformCoordinate(to, from);
}

void RFLSetCoordinate(RFLCoordinateType u, RFLCoordinateType f) {

    auto extractBytes = [](RFLCoordinateType c) -> std::array<u8, 3> {
        return {
            static_cast<u8>((c >> 24) & 0xFF),
            static_cast<u8>((c >> 16) & 0xFF),
            static_cast<u8>((c >> 8) & 0xFF)
        };
    };

    auto makeCoordType = [](const std::array<u8, 3>& b) -> RFLCoordinateType {
        return static_cast<RFLCoordinateType>(
            (b[0] << 24) | (b[1] << 16) | (b[2] << 8)
        );
    };

    auto uu = extractBytes(u);
    auto uf = extractBytes(f);
    std::array<u8, 3> ur;

    ur[0] = (uu[1] * uf[2]) - (uu[2] * uf[1]);
    ur[1] = (uu[2] * uf[0]) - (uu[0] * uf[2]);
    ur[2] = (uu[0] * uf[1]) - (uu[1] * uf[0]);

    RFLCoordinateType r = makeCoordType(ur);

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

    constexpr u32 GX_TEXOBJ_SIZE = sizeof(GXTexObj);

    const u32 exprNum = countExpressions(exprFlags);
    const u32 texSize = getMaskBufSize(res);

    return roundUp(sizeof(CharModelInternal), 32) +
           roundUp(exprNum * GX_TEXOBJ_SIZE, 32) +
           roundUp(sizeof(CharModelRes), 32) +
           roundUp(texSize * exprNum, 32);
}


namespace {

struct Rgba {
    float r, g, b, a;
};

struct PartTex {
    int w = 0;
    int h = 0;
    std::vector<Rgba> px;
};

bool decodePartTex(ArcID arc, u16 file, PartTex& out) {
    u32 size = 0;
    const u8* f = getResourceLoader().getFile(arc, file, &size);
    if (!f || size < 32) {
        return false;
    }
    u8 fmt = f[0];
    out.w = revointernal::readBE16(f + 2);
    out.h = revointernal::readBE16(f + 4);
    u32 ofs = revointernal::readBE<u32>(f + 28);
    const u8* d = f + ofs;
    out.px.assign(out.w * out.h, Rgba{0, 0, 0, 0});
    u32 pos = 0;
    int bw = 4, bh = 4;
    if (fmt == 0) { bw = 8; bh = 8; }
    else if (fmt == 2) { bw = 8; bh = 4; }
    else if (fmt != 5) return false;
    for (int by = 0; by < out.h; by += bh) {
        for (int bx = 0; bx < out.w; bx += bw) {
            for (int y = 0; y < bh; y++) {
                for (int x = 0; x < bw; x++) {
                    Rgba c{0, 0, 0, 0};
                    if (fmt == 5) {
                        u16 v = revointernal::readBE16(d + pos);
                        pos += 2;
                        if (v & 0x8000) {
                            c = {((v >> 10) & 31) / 31.0f, ((v >> 5) & 31) / 31.0f, (v & 31) / 31.0f, 1.0f};
                        } else {
                            c = {((v >> 8) & 15) / 15.0f, ((v >> 4) & 15) / 15.0f, (v & 15) / 15.0f, ((v >> 12) & 7) / 7.0f};
                        }
                    } else if (fmt == 0) {
                        u8 b = d[pos + (y * bw + x) / 2];
                        float i = ((x & 1) ? (b & 15) : (b >> 4)) / 15.0f;
                        c = {i, i, i, i};
                    } else {
                        u8 b = d[pos++];
                        float i = (b & 15) / 15.0f;
                        float a = (b >> 4) / 15.0f;
                        c = {i, i, i, a};
                    }
                    if (bx + x < out.w && by + y < out.h) {
                        out.px[(by + y) * out.w + bx + x] = c;
                    }
                }
            }
            if (fmt == 0) pos += 32;
        }
    }
    return true;
}

Rgba samplePart(const PartTex& t, float x, float y) {
    x -= 0.5f;
    y -= 0.5f;
    int x0 = (int)std::floor(x), y0 = (int)std::floor(y);
    float fx = x - x0, fy = y - y0;
    auto at = [&](int xx, int yy) {
        if (xx < 0 || yy < 0 || xx >= t.w || yy >= t.h) return Rgba{0, 0, 0, 0};
        return t.px[yy * t.w + xx];
    };
    Rgba c00 = at(x0, y0), c10 = at(x0 + 1, y0), c01 = at(x0, y0 + 1), c11 = at(x0 + 1, y0 + 1);
    auto mix = [&](float a, float b, float c, float d) {
        return (a * (1 - fx) + b * fx) * (1 - fy) + (c * (1 - fx) + d * fx) * fy;
    };
    return {mix(c00.r, c10.r, c01.r, c11.r), mix(c00.g, c10.g, c01.g, c11.g), mix(c00.b, c10.b, c01.b, c11.b), mix(c00.a, c10.a, c01.a, c11.a)};
}

enum class TintMode { Channels, Intensity };

struct MaskCanvas {
    int res;
    std::vector<Rgba> px;
};

constexpr float kScaleX = 0.88961464f;
constexpr float kScaleY = 0.9276675f;

enum Origin { OriginCenter = 0, OriginRight = 1, OriginLeft = 2 };

void drawPart(MaskCanvas& cv, const PartTex& t, float x, float y, float w, float h, float angleDeg, Origin origin, TintMode mode, const float c0[3],
              const float c1[3], const float c2[3]) {
    float baseX = origin == OriginCenter ? -0.5f : (origin == OriginRight ? -1.0f : 0.0f);
    float ext = (w + h) * kScaleX + 4.0f;
    int x0 = std::max(0, (int)(x - ext)), x1 = std::min(cv.res - 1, (int)(x + ext));
    int y0 = std::max(0, (int)(y - ext)), y1 = std::min(cv.res - 1, (int)(y + ext));
    float rad = angleDeg * 3.14159265f / 180.0f;
    float cs = std::cos(rad), sn = std::sin(rad);
    for (int py = y0; py <= y1; py++) {
        for (int px = x0; px <= x1; px++) {
            float dx = (px + 0.5f - x) / kScaleX;
            float dy = (py + 0.5f - y) / kScaleY;
            float rx = dx * cs + dy * sn;
            float ry = -dx * sn + dy * cs;
            float qx = rx / w;
            float qy = ry / h;
            if (qx < baseX || qx > baseX + 1.0f || qy < -0.5f || qy > 0.5f) continue;
            float u = origin == OriginLeft ? 1.0f - qx : qx - baseX;
            float v = qy + 0.5f;
            Rgba s = samplePart(t, u * t.w, v * t.h);
            float sa = s.a;
            if (sa <= 0.0f) continue;
            float col[3];
            if (mode == TintMode::Channels) {
                for (int k = 0; k < 3; k++) {
                    col[k] = s.r * c0[k] + s.b * c1[k] + s.g * c2[k];
                }
            } else {
                for (int k = 0; k < 3; k++) col[k] = c0[k];
            }
            Rgba& d = cv.px[py * cv.res + px];
            float oa = sa + d.a * (1 - sa);
            d.r = (col[0] * sa + d.r * d.a * (1 - sa)) / oa;
            d.g = (col[1] * sa + d.g * d.a * (1 - sa)) / oa;
            d.b = (col[2] * sa + d.b * d.a * (1 - sa)) / oa;
            d.a = oa;
        }
    }
}

void toF(GXColor c, float out[3]) {
    out[0] = c.r / 255.0f;
    out[1] = c.g / 255.0f;
    out[2] = c.b / 255.0f;
}

constexpr u8 kEyeRotOffset[50] = {29, 28, 28, 28, 29, 28, 28, 28, 29, 28, 28, 28, 28, 29, 29, 28, 28, 28, 29, 29, 28, 29, 28, 29, 29,
                                  28, 29, 28, 28, 29, 28, 28, 28, 29, 29, 29, 28, 28, 29, 29, 29, 28, 28, 29, 29, 29, 29, 29, 28, 28};
constexpr u8 kEyebrowRotOffset[24] = {26, 26, 27, 25, 26, 25, 26, 25, 28, 25, 26, 24, 27, 27, 26, 26, 25, 25, 26, 26, 27, 26, 25, 27};
constexpr GXColor kEyeColor1[6] = {{0, 0, 0, 255}, {124, 128, 128, 255}, {112, 80, 64, 255}, {112, 110, 64, 255}, {88, 104, 184, 255}, {72, 128, 104, 255}};
constexpr GXColor kMouthColor0[3] = {{190, 78, 38, 255}, {216, 48, 40, 255}, {207, 68, 71, 255}};
constexpr GXColor kMouthColor1[3] = {{113, 42, 4, 255}, {120, 21, 16, 255}, {126, 37, 40, 255}};
constexpr GXColor kMoleColor = {18, 15, 15, 255};

float rot2ang(int rotate) {
    return (360.0f / 32.0f) * (float)(rotate % 32);
}

float scale2dim(int scale) {
    return 1.0f + 0.4f * scale;
}

void convertCharInfo(const RFLiCharInfo& in, CharInfo* out) {
    std::memset(out, 0, sizeof(*out));
    out->facelineType = in.faceline.type;
    out->facelineColor = in.faceline.color;
    out->facelineTexture = in.faceline.texture;
    out->hairType = in.hair.type;
    out->hairColor = in.hair.color;
    out->hairFlip = in.hair.flip;
    out->noseType = in.nose.type;
    out->noseScale = in.nose.scale;
    out->noseY = in.nose.y;
    out->beardType = in.beard.type;
    out->beardColor = in.beard.color;
    out->beardScale = in.beard.scale;
    out->beardY = in.beard.y;
    out->beardMustache = in.beard.mustache;
    out->glassType = in.glass.type;
    out->glassColor = in.glass.color;
    out->glassScale = in.glass.scale;
    out->glassY = in.glass.y;
    out->personalColor = in.personal.color;
}

void bindArrays(const s16* pos, const s16* nrm, const s16* txc, u32 posSize, u32 nrmSize, u32 txcSize) {
#if MIIVOLUTION_AURORA
    GXSetArray(GX_VA_POS, pos, posSize, 6, true);
    GXSetArray(GX_VA_NRM, nrm, nrmSize, 6, true);
#else
    GXSetArray(GX_VA_POS, pos, 6);
    GXSetArray(GX_VA_NRM, nrm, 6);
#endif
    if (txc) {
#if MIIVOLUTION_AURORA
        GXSetArray(GX_VA_TEX0, txc, txcSize, 4, true);
#else
        GXSetArray(GX_VA_TEX0, txc, 4);
#endif
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
    } else {
        GXSetVtxDesc(GX_VA_TEX0, GX_NONE);
    }
}

void callDl(const u8* dl, u32 size, const s16* pos, const s16* nrm, const s16* txc,
            u32 posSize, u32 nrmSize, u32 txcSize) {
    if (!dl || size == 0) return;
    bindArrays(pos, nrm, txc, posSize, nrmSize, txcSize);
    GXCallDisplayList(dl, size);
}

}

extern "C" RFLErrcode RFLiPickupCharInfo(void* info, RFLDataSource source, RFLMiddleDB* db, u16 index);

RFLErrcode RFLInitCharModel(RFLCharModel* model, RFLDataSource src,
                            RFLMiddleDB* db, u16 id, void* work,
                            RFLResolution res, u32 exprFlags) {
    if (!model || !work) {
        return RFLErrcode_WrongParam;
    }

    if (!RFLAvailable()) {
        return RFLErrcode_NotAvailable;
    }

    alignas(16) u8 charInfoBuf[sizeof(RFLiCharInfo) + 16];
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
    workPtr += roundUp(sizeof(CharModelRes), 32);

    const RFLiCharInfo& info = *reinterpret_cast<RFLiCharInfo*>(charInfoBuf);
    CharInfo ci;
    convertCharInfo(info, &ci);
    RFLiInitCharModelRes(internal->res, &ci);
    if (info.glass.type == 0) {
        internal->res->glassesDlSize = 0;
    }

    int topRes = 64;
    u32 resBits = static_cast<u32>(res);
    if (resBits & 256) topRes = 256;
    else if (resBits & 128) topRes = 128;
    const u32 maskStride = getMaskBufSize(res);
    for (u32 i = 0; i < RFLExp_Max; i++) {
        if (!internal->maskTexObj[i]) continue;
        composeMask(workPtr, topRes, info, i == RFLExp_Blink);
        GXInitTexObj(internal->maskTexObj[i], workPtr, topRes, topRes, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP, GX_FALSE);
        GXInitTexObjLOD(internal->maskTexObj[i], GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
        workPtr += roundUp(maskStride, 32);
    }

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
    const auto sel =
        static_cast<GXTevKColorSel>(
            static_cast<int>(setting->tevKColorID) +
            static_cast<int>(GX_TEV_KCSEL_K0)
        );

    GXSetTevKColorSel(GX_TEVSTAGE0, sel);
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

    CharModelRes* r = internal->res;
    if (!r) return;

#if MIIVOLUTION_RAINFALL
    GXSetArrayNativeEndian_PC(GX_TRUE);
#endif

    GXColor face = getFacelineColor(r->facelineColor);
    GXSetTevKColor(setting->tevKColorID, face);
    callDl(r->faceDl, r->faceDlSize, r->faceVtxPos, r->faceVtxNrm, r->faceVtxTxc,
           sizeof(r->faceVtxPos), sizeof(r->faceVtxNrm), sizeof(r->faceVtxTxc));
    callDl(r->noseDl, r->noseDlSize, r->noseVtxPos, r->noseVtxNrm, nullptr,
           sizeof(r->noseVtxPos), sizeof(r->noseVtxNrm), 0);
    callDl(r->foreheadDl, r->foreheadDlSize, r->foreheadVtxPos, r->foreheadVtxNrm, nullptr,
           sizeof(r->capVtxPos), sizeof(r->capVtxNrm), 0);

    GXSetTevKColor(setting->tevKColorID, getHairColor(r->hairColor));
    callDl(r->hairDl, r->hairDlSize, r->hairVtxPos, r->hairVtxNrm, nullptr,
           sizeof(r->capVtxPos), sizeof(r->capVtxNrm), 0);

    GXSetTevKColor(setting->tevKColorID, getBeardColor(r->beardColor));
    callDl(r->beardDl, r->beardDlSize, r->beardVtxPos, r->beardVtxNrm, nullptr,
           sizeof(r->beardVtxPos), sizeof(r->beardVtxNrm), 0);

#if MIIVOLUTION_RAINFALL
    GXSetArrayNativeEndian_PC(GX_FALSE);
#endif
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

    CharModelRes* r = internal->res;
    if (!r) return;
#if MIIVOLUTION_RAINFALL
    GXSetArrayNativeEndian_PC(GX_TRUE);
#endif
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);

    GXLoadTexObj(&r->faceTexObj, setting->texMapID);
    callDl(r->faceDl, r->faceDlSize, r->faceVtxPos, r->faceVtxNrm, r->faceVtxTxc,
           sizeof(r->faceVtxPos), sizeof(r->faceVtxNrm), sizeof(r->faceVtxTxc));

    GXTexObj* mask = internal->maskTexObj[internal->currentExpression];
    if (!mask) mask = internal->maskTexObj[RFLExp_Normal];
    if (mask) {
        GXLoadTexObj(mask, setting->texMapID);
        callDl(r->maskDl, r->maskDlSize, r->maskVtxPos, r->maskVtxNrm, r->maskVtxTxc,
               sizeof(r->maskVtxPos), sizeof(r->maskVtxNrm), sizeof(r->maskVtxTxc));
    }

    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_KONST);

    if (r->capDlSize > 0) {
        GXSetTevKColor(setting->tevKColorID, getHairColor(r->hairColor));
        GXLoadTexObj(&r->capTexObj, setting->texMapID);
        callDl(r->capDl, r->capDlSize, r->capVtxPos, r->capVtxNrm, r->capVtxTxc,
               sizeof(r->capVtxPos), sizeof(r->capVtxNrm), sizeof(r->capVtxTxc));
    }

    if (r->noselineDlSize > 0) {
        GXSetTevKColor(setting->tevKColorID, GXColor{40, 24, 16, 255});
        GXLoadTexObj(&r->noseTexObj, setting->texMapID);
        callDl(r->noselineDl, r->noselineDlSize, r->noselineVtxPos, r->noselineVtxNrm, r->noselineVtxTxc,
               sizeof(r->noselineVtxPos), sizeof(r->noselineVtxNrm), sizeof(r->noselineVtxTxc));
    }

    if (r->glassesDlSize > 0) {
        GXSetTevKColor(setting->tevKColorID, getGlassColor(r->glassesColor));
        GXLoadTexObj(&r->glassesTexObj, setting->texMapID);
        callDl(r->glassesDl, r->glassesDlSize, r->glassesVtxPos, r->glassesVtxNrm, r->glassesVtxTxc,
               sizeof(r->glassesVtxPos), sizeof(r->glassesVtxNrm), sizeof(r->glassesVtxTxc));
    }

    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
#if MIIVOLUTION_RAINFALL
    GXSetArrayNativeEndian_PC(GX_FALSE);
#endif
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

void RFLiInitShapeRes(ShapeRes* shape) {
    using namespace rvlfacelib;

    bool skipTxc = shape->part == PartsShp::Forehead ||
                   shape->part == PartsShp::Hair ||
                   shape->part == PartsShp::Beard ||
                   shape->part == PartsShp::Nose;

    u32 fileSize = RFLiGetShapeSize(static_cast<u32>(shape->part), shape->file);
    void* res = RFLiAlloc32(fileSize);
    if (!res) {
        shape->numVtxPos = 0;
        shape->numVtxNrm = 0;
        shape->numVtxTxc = 0;
        shape->dlSize = 0;
        return;
    }

    RFLiLoadShape(static_cast<u32>(shape->part), shape->file, res);

    auto* ptr8 = static_cast<u8*>(res);
    ptr8 += sizeof(u32);

    if (shape->part == PartsShp::Faceline) {
        Vec* dstVecs[3] = {shape->noseTrans, shape->beardTrans, shape->hairTrans};
        for (Vec* v : dstVecs) {
            u32 raw[3];
            for (int k = 0; k < 3; k++) {
                raw[k] = revointernal::readBE<u32>(ptr8 + k * 4);
            }
            std::memcpy(v, raw, sizeof(Vec));
            ptr8 += sizeof(Vec);
        }
    }

    u16 numVtxPos = revointernal::readBE16(ptr8);
    if (numVtxPos == 0) {
        shape->numVtxPos = 0;
        shape->numVtxNrm = 0;
        shape->numVtxTxc = 0;
        shape->dlSize = 0;
        RFLiFree(res);
        return;
    }

    shape->numVtxPos = numVtxPos;
    ptr8 += sizeof(u16);

    {
        u32 byteSize = SIZE_VTX_POS(shape->numVtxPos);
        auto* ptr16 = reinterpret_cast<s16*>(ptr8);

        if (shape->transform) {
            s32 s = static_cast<s32>(256.0f * shape->posScale);
            s32 tx = static_cast<s32>(256.0f * shape->posTrans->x);
            s32 ty = static_cast<s32>(256.0f * shape->posTrans->y);
            s32 tz = static_cast<s32>(256.0f * shape->posTrans->z);

            for (u16 i = 0; i < shape->numVtxPos; i++) {
                s16 temp[3];
                s16 srcX = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[0]));
                s16 srcY = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[1]));
                s16 srcZ = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[2]));

                if (shape->flipX) {
                    temp[0] = static_cast<s16>(tx + ((-srcX * s) >> 8));
                } else {
                    temp[0] = static_cast<s16>(tx + ((srcX * s) >> 8));
                }

                temp[1] = static_cast<s16>(ty + ((srcY * s) >> 8));
                temp[2] = static_cast<s16>(tz + ((srcZ * s) >> 8));

                RFLiTransformCoordinate(&shape->vtxPosBuf[i * VTX_COORDS_IN_POS], temp);
                ptr16 += VTX_COORDS_IN_POS;
            }
        } else if (shape->flipX) {
            for (u16 i = 0; i < shape->numVtxPos; i++) {
                s16 temp[3];
                s16 srcX = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[0]));
                s16 srcY = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[1]));
                s16 srcZ = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[2]));

                temp[0] = -srcX;
                temp[1] = srcY;
                temp[2] = srcZ;

                RFLiTransformCoordinate(&shape->vtxPosBuf[i * VTX_COORDS_IN_POS], temp);
                ptr16 += VTX_COORDS_IN_POS;
            }
        } else {
            for (u16 i = 0; i < shape->numVtxPos; i++) {
                s16 temp[3];
                temp[0] = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[0]));
                temp[1] = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[1]));
                temp[2] = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[2]));

                RFLiTransformCoordinate(&shape->vtxPosBuf[i * VTX_COORDS_IN_POS], temp);
                ptr16 += VTX_COORDS_IN_POS;
            }
        }

        ptr8 += byteSize;
    }

    shape->numVtxNrm = revointernal::readBE16(ptr8);
    ptr8 += sizeof(u16);

    {
        auto* ptr16 = reinterpret_cast<s16*>(ptr8);
        u32 byteSize = SIZE_VTX_NRM(shape->numVtxNrm);

        if (shape->flipX) {
            for (u16 i = 0; i < shape->numVtxNrm; i++) {
                s16 temp[3];
                s16 srcX = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[0]));
                s16 srcY = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[1]));
                s16 srcZ = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[2]));

                temp[0] = -srcX;
                temp[1] = srcY;
                temp[2] = srcZ;

                RFLiTransformCoordinate(&shape->vtxNrmBuf[i * VTX_COORDS_IN_NRM], temp);
                ptr16 += VTX_COORDS_IN_NRM;
            }
        } else {
            for (u16 i = 0; i < shape->numVtxNrm; i++) {
                s16 temp[3];
                temp[0] = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[0]));
                temp[1] = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[1]));
                temp[2] = revointernal::readBE<s16>(reinterpret_cast<u8*>(&ptr16[2]));

                RFLiTransformCoordinate(&shape->vtxNrmBuf[i * VTX_COORDS_IN_NRM], temp);
                ptr16 += VTX_COORDS_IN_NRM;
            }
        }

        ptr8 += byteSize;
    }

    if (skipTxc) {
        shape->numVtxTxc = 0;
    } else {
        shape->numVtxTxc = revointernal::readBE16(ptr8);
        ptr8 += sizeof(u16);

        u32 byteSize = SIZE_VTX_TXC(shape->numVtxTxc);
        for (u32 k = 0; k < shape->numVtxTxc * VTX_COORDS_IN_TXC; k++) {
            shape->vtxTxcBuf[k] = revointernal::readBE<s16>(ptr8 + k * 2);
        }
        ptr8 += byteSize;
    }

    {
        s32 primitiveNum = *ptr8++;

        GXBeginDisplayList(shape->dlBuf, shape->dlBufSize);

        for (s32 i = 0; i < primitiveNum; i++) {
            u16 vtxNum = *ptr8++;
            auto prim = static_cast<GXPrimitive>(*ptr8++);

            GXBegin(prim, GX_VTXFMT0, vtxNum);
            for (u16 j = 0; j < vtxNum; j++) {
                GXPosition1x8(*ptr8++);
                GXNormal1x8(*ptr8++);

                if (!skipTxc) {
                    GXTexCoord1x8(*ptr8++);
                }
            }
            GXEnd();
        }

        shape->dlSize = GXEndDisplayList();
    }

    RFLiFree(res);
}

void RFLiInitTexRes(GXTexObj* texObj, u32 part, u16 file, void* buffer) {
    using namespace rvlfacelib;

    u32 texSize = RFLiGetShpTexSize(part, file);
    if (texSize < sizeof(Texture)) {
        return;
    }
    auto* tex = static_cast<u8*>(RFLiAlloc32(texSize));
    if (!tex) {
        return;
    }

    RFLiLoadShpTexture(part, file, tex);

    u8 fmt = tex[0];
    u16 width = revointernal::readBE16(tex + 2);
    u16 height = revointernal::readBE16(tex + 4);
    u8 wrapS = tex[6];
    u8 wrapT = tex[7];
    u32 imageOfs = revointernal::readBE<u32>(tex + 28);

    u32 imgSize = 0;
    switch (static_cast<PartsShpTex>(part)) {
    case PartsShpTex::Face:
        imgSize = height * width * 2;
        break;
    case PartsShpTex::Cap:
    case PartsShpTex::Noseline:
        imgSize = height * width / 2;
        break;
    case PartsShpTex::Glass:
        imgSize = height * width;
        break;
    default:
        break;
    }

    if (imgSize > 0 && imageOfs + imgSize <= texSize) {
        std::memcpy(buffer, tex + imageOfs, imgSize);
        GXInitTexObj(texObj, buffer, width, height, static_cast<GXTexFmt>(fmt), static_cast<GXTexWrapMode>(wrapS),
                     static_cast<GXTexWrapMode>(wrapT), FALSE);
        GXInitTexObjLOD(texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, FALSE, FALSE, GX_ANISO_1);
    }

    RFLiFree(tex);
}

void RFLiInitCharModelRes(void* resPtr, const CharInfo* info) {
    using namespace rvlfacelib;

    auto* res = static_cast<CharModelRes*>(resPtr);

    Vec noseTrans;
    Vec beardTrans;
    Vec hairTrans;

    // GXSetMisc(GX_MT_XF_FLUSH, 0);
    // GXSetMisc(GX_MT_DL_SAVE_CONTEXT, 1);

    // Faceline shape
    {
        ShapeRes arg{};
        arg.part = PartsShp::Faceline;
        arg.file = info->facelineType;
        arg.vtxPosBuf = res->faceVtxPos;
        arg.vtxNrmBuf = res->faceVtxNrm;
        arg.vtxTxcBuf = res->faceVtxTxc;
        arg.dlBuf = res->faceDl;
        arg.vtxPosBufSize = NUM_VTX_POS(sizeof(res->faceVtxPos));
        arg.vtxNrmBufSize = NUM_VTX_NRM(sizeof(res->faceVtxNrm));
        arg.vtxTxcBufSize = NUM_VTX_TXC(sizeof(res->faceVtxTxc));
        arg.dlBufSize = sizeof(res->faceDl);
        arg.noseTrans = &noseTrans;
        arg.beardTrans = &beardTrans;
        arg.hairTrans = &hairTrans;
        arg.flipX = FALSE;
        arg.transform = FALSE;
        RFLiInitShapeRes(&arg);

        res->faceDlSize = arg.dlSize;
    }

    // Cap shape
    {
        ShapeRes arg{};
        arg.part = PartsShp::Cap;
        arg.file = info->hairType;
        arg.vtxPosBuf = res->capVtxPos;
        arg.vtxNrmBuf = res->capVtxNrm;
        arg.vtxTxcBuf = res->capVtxTxc;
        arg.dlBuf = res->capDl;
        arg.vtxPosBufSize = NUM_VTX_POS(sizeof(res->capVtxPos));
        arg.vtxNrmBufSize = NUM_VTX_NRM(sizeof(res->capVtxNrm));
        arg.vtxTxcBufSize = NUM_VTX_TXC(sizeof(res->capVtxTxc));
        arg.dlBufSize = sizeof(res->capDl);
        arg.flipX = info->hairFlip ? TRUE : FALSE;
        arg.transform = TRUE;
        arg.posScale = 1.0f;
        arg.posTrans = &hairTrans;
        RFLiInitShapeRes(&arg);

        res->capDlSize = arg.dlSize;

        res->hairVtxPos = reinterpret_cast<s16*>(reinterpret_cast<u8*>(res->capVtxPos) +
                         (arg.numVtxPos * 4 - arg.numVtxPos) * 2);
        res->hairVtxNrm = reinterpret_cast<s16*>(reinterpret_cast<u8*>(res->capVtxNrm) +
                         (arg.numVtxNrm * 4 - arg.numVtxNrm) * 2);

        res->hairDl = res->noseDl + roundUp(arg.dlSize, 32) + offsetof(CharModelRes, capDl);
    }

    // Hair shape
    {
        ShapeRes arg{};
        arg.part = PartsShp::Hair;
        arg.file = info->hairType;
        arg.vtxPosBuf = res->hairVtxPos;
        arg.vtxNrmBuf = res->hairVtxNrm;
        arg.dlBuf = res->hairDl;

        arg.vtxPosBufSize = NUM_VTX_POS(sizeof(res->capVtxPos)) -
            ((reinterpret_cast<uintptr_t>(res->hairVtxPos) - reinterpret_cast<uintptr_t>(res->capVtxPos)) /
             VTX_COORD_SIZE) / VTX_COORDS_IN_POS;

        arg.vtxNrmBufSize = NUM_VTX_NRM(sizeof(res->capVtxNrm)) -
            ((reinterpret_cast<uintptr_t>(res->hairVtxNrm) - reinterpret_cast<uintptr_t>(res->capVtxNrm)) /
             VTX_COORD_SIZE) / VTX_COORDS_IN_NRM;

        arg.dlBufSize = sizeof(res->capDl) -
            (reinterpret_cast<uintptr_t>(res->hairDl) - reinterpret_cast<uintptr_t>(res->capDl));

        arg.flipX = info->hairFlip ? TRUE : FALSE;
        arg.transform = TRUE;
        arg.posScale = 1.0f;
        arg.posTrans = &hairTrans;
        RFLiInitShapeRes(&arg);

        res->hairDlSize = arg.dlSize;

        res->foreheadVtxPos = reinterpret_cast<s16*>(reinterpret_cast<u8*>(res->hairVtxPos) +
                             (arg.numVtxPos * 4 - arg.numVtxPos) * 2);
        res->foreheadVtxNrm = reinterpret_cast<s16*>(reinterpret_cast<u8*>(res->hairVtxNrm) +
                             (arg.numVtxNrm * 4 - arg.numVtxNrm) * 2);

        res->foreheadDl = res->hairDl + roundUp(arg.dlSize, 32);
        res->flipHair = info->hairFlip ? TRUE : FALSE;
    }

    // Forehead shape
    {
        ShapeRes arg{};
        arg.part = PartsShp::Forehead;
        arg.file = info->hairType;
        arg.vtxPosBuf = res->foreheadVtxPos;
        arg.vtxNrmBuf = res->foreheadVtxNrm;
        arg.dlBuf = res->foreheadDl;

        arg.vtxPosBufSize = NUM_VTX_POS(sizeof(res->capVtxPos)) -
            ((reinterpret_cast<uintptr_t>(res->foreheadVtxPos) - reinterpret_cast<uintptr_t>(res->capVtxPos)) /
             VTX_COORD_SIZE) / VTX_COORDS_IN_POS;

        arg.vtxNrmBufSize = NUM_VTX_NRM(sizeof(res->capVtxNrm)) -
            ((reinterpret_cast<uintptr_t>(res->foreheadVtxNrm) - reinterpret_cast<uintptr_t>(res->capVtxNrm)) /
             VTX_COORD_SIZE) / VTX_COORDS_IN_NRM;

        arg.dlBufSize = sizeof(res->capDl) -
            (reinterpret_cast<uintptr_t>(res->foreheadDl) - reinterpret_cast<uintptr_t>(res->capDl));

        arg.flipX = info->hairFlip ? TRUE : FALSE;
        arg.transform = TRUE;
        arg.posScale = 1.0f;
        arg.posTrans = &hairTrans;
        RFLiInitShapeRes(&arg);

        res->foreheadDlSize = arg.dlSize;
    }

    // Beard shape
    {
        ShapeRes arg{};
        arg.part = PartsShp::Beard;
        arg.file = info->beardType;
        arg.vtxPosBuf = res->beardVtxPos;
        arg.vtxNrmBuf = res->beardVtxNrm;
        arg.dlBuf = res->beardDl;
        arg.vtxPosBufSize = NUM_VTX_POS(sizeof(res->beardVtxPos));
        arg.vtxNrmBufSize = NUM_VTX_NRM(sizeof(res->beardVtxNrm));
        arg.dlBufSize = sizeof(res->beardDl);
        arg.flipX = FALSE;
        arg.transform = TRUE;
        arg.posScale = 1.0f;
        arg.posTrans = &beardTrans;
        RFLiInitShapeRes(&arg);

        res->beardDlSize = arg.dlSize;
    }

    // Nose and noseline shapes
    {
        f32 scale = 0.4f + 0.175f * info->noseScale;
        Vec trans;
        trans.x = noseTrans.x;
        trans.y = noseTrans.y + -1.5f * (info->noseY - 8);
        trans.z = noseTrans.z;

        // Nose shape
        {
            ShapeRes arg{};
            arg.part = PartsShp::Nose;
            arg.file = info->noseType;
            arg.vtxPosBuf = res->noseVtxPos;
            arg.vtxNrmBuf = res->noseVtxNrm;
            arg.dlBuf = res->noseDl;
            arg.vtxPosBufSize = NUM_VTX_POS(sizeof(res->noseVtxPos));
            arg.vtxNrmBufSize = NUM_VTX_NRM(sizeof(res->noseVtxNrm));
            arg.dlBufSize = sizeof(res->noseDl);
            arg.flipX = FALSE;
            arg.transform = TRUE;
            arg.posScale = scale;
            arg.posTrans = &trans;
            RFLiInitShapeRes(&arg);

            res->noseDlSize = arg.dlSize;
        }

        // Noseline shape
        {
            ShapeRes arg{};
            arg.part = PartsShp::Noseline;
            arg.file = info->noseType;
            arg.vtxPosBuf = res->noselineVtxPos;
            arg.vtxNrmBuf = res->noselineVtxNrm;
            arg.vtxTxcBuf = res->noselineVtxTxc;
            arg.dlBuf = res->noselineDl;
            arg.vtxPosBufSize = NUM_VTX_POS(sizeof(res->noselineVtxPos));
            arg.vtxNrmBufSize = NUM_VTX_NRM(sizeof(res->noselineVtxNrm));
            arg.vtxTxcBufSize = NUM_VTX_TXC(sizeof(res->noselineVtxTxc));
            arg.dlBufSize = sizeof(res->noselineDl);
            arg.flipX = FALSE;
            arg.transform = TRUE;
            arg.posScale = scale;
            arg.posTrans = &trans;
            RFLiInitShapeRes(&arg);

            res->noselineDlSize = arg.dlSize;
        }
    }

    // Mask shape
    {
        ShapeRes arg{};
        arg.part = PartsShp::Mask;
        arg.file = info->facelineType;
        arg.vtxPosBuf = res->maskVtxPos;
        arg.vtxNrmBuf = res->maskVtxNrm;
        arg.vtxTxcBuf = res->maskVtxTxc;
        arg.dlBuf = res->maskDl;
        arg.vtxPosBufSize = NUM_VTX_POS(sizeof(res->maskVtxPos));
        arg.vtxNrmBufSize = NUM_VTX_NRM(sizeof(res->maskVtxNrm));
        arg.vtxTxcBufSize = NUM_VTX_TXC(sizeof(res->maskVtxTxc));
        arg.dlBufSize = sizeof(res->maskDl);
        arg.flipX = FALSE;
        arg.transform = FALSE;
        RFLiInitShapeRes(&arg);

        res->maskDlSize = arg.dlSize;
    }

    // Glasses shape
    {
        f32 scale = 0.15f * info->glassScale + 0.4f;
        Vec trans;
        trans.x = noseTrans.x;
        trans.y = 5.0f + noseTrans.y + -1.5f * (info->glassY - 11);
        trans.z = 2.0f + noseTrans.z;

        ShapeRes arg{};
        arg.part = PartsShp::Glass;
        arg.file = 0;
        arg.vtxPosBuf = res->glassesVtxPos;
        arg.vtxNrmBuf = res->glassesVtxNrm;
        arg.vtxTxcBuf = res->glassesVtxTxc;
        arg.dlBuf = res->glassesDl;
        arg.vtxPosBufSize = NUM_VTX_POS(sizeof(res->glassesVtxPos));
        arg.vtxNrmBufSize = NUM_VTX_NRM(sizeof(res->glassesVtxNrm));
        arg.vtxTxcBufSize = NUM_VTX_TXC(sizeof(res->glassesVtxTxc));
        arg.dlBufSize = sizeof(res->glassesDl);
        arg.flipX = FALSE;
        arg.transform = TRUE;
        arg.posScale = scale;
        arg.posTrans = &trans;
        RFLiInitShapeRes(&arg);

        res->glassesDlSize = arg.dlSize;
    }

    // Initialize textures
    RFLiInitTexRes(&res->faceTexObj, static_cast<u32>(PartsShpTex::Face),
                   info->facelineTexture, res->faceTex);

    if (res->capDlSize > 0) {
        RFLiInitTexRes(&res->capTexObj, static_cast<u32>(PartsShpTex::Cap),
                       info->hairType, res->capTex);
    }

    if (res->noselineDlSize > 0) {
        RFLiInitTexRes(&res->noseTexObj, static_cast<u32>(PartsShpTex::Noseline),
                       info->noseType, res->noseTex);
    }

    RFLiInitTexRes(&res->glassesTexObj, static_cast<u32>(PartsShpTex::Glass),
                   info->glassType, res->glassesTex);

    // Set colors
    res->facelineColor = info->facelineColor;
    res->hairColor = info->hairColor;
    res->beardColor = info->beardColor;
    res->glassesColor = info->glassColor;
    res->favoriteColor = info->personalColor;
}

}

void rvlfacelib::composeMask(u8* dst, int res, const RFLiCharInfo& ci, bool blink) {
    MaskCanvas cv{res, std::vector<Rgba>(res * res, Rgba{0, 0, 0, 0})};
    const float unit = res / 64.0f;
    const float white[3] = {1, 1, 1};

    int eyeType = blink ? 48 : ci.eye.type;
    int eyeRotate = ci.eye.rotate;
    if (blink) {
        int change = (int)kEyeRotOffset[ci.eye.type < 50 ? ci.eye.type : 0] - (int)kEyeRotOffset[48];
        eyeRotate = std::min(7, std::max(0, eyeRotate + change));
    }
    int eyeY = ci.eye.y;
    int browY = ci.eyebrow.y;

    PartTex t;

    float eyeX = kScaleX * ci.eye.x;
    float eyeYp = 18.451525f + 1.1600001f * kScaleY * eyeY;
    float eyeW = (342.0f / 64.0f) * scale2dim(ci.eye.scale) * unit;
    float eyeH = (288.0f / 64.0f) * scale2dim(ci.eye.scale) * unit;
    float eyeA = rot2ang(eyeRotate + kEyeRotOffset[eyeType < 50 ? eyeType : 0]);

    float browX = kScaleX * ci.eyebrow.x;
    float browYp = 16.549807f + 1.1600001f * kScaleY * browY;
    float browW = (324.0f / 64.0f) * scale2dim(ci.eyebrow.scale) * unit;
    float browH = (288.0f / 64.0f) * scale2dim(ci.eyebrow.scale) * unit;
    float browA = rot2ang(ci.eyebrow.rotate + kEyebrowRotOffset[ci.eyebrow.type < 24 ? ci.eyebrow.type : 0]);

    float mouthYp = 29.25885f + 1.1600001f * kScaleY * ci.mouth.y;
    float mouthW = (396.0f / 64.0f) * scale2dim(ci.mouth.scale) * unit;
    float mouthH = (288.0f / 64.0f) * scale2dim(ci.mouth.scale) * unit;

    float mustYp = 31.763554f + 1.1600001f * kScaleY * ci.beard.y;
    float mustW = (288.0f / 64.0f) * scale2dim(ci.beard.scale) * unit;
    float mustH = (576.0f / 64.0f) * scale2dim(ci.beard.scale) * unit;

    float moleX = 17.766165f + 2.0f * kScaleX * ci.mole.x;
    float moleY = 17.95986f + 1.1600001f * kScaleY * ci.mole.y;
    float moleSz = scale2dim(ci.mole.scale) * unit;

    if (ci.beard.mustache > 0 && decodePartTex(ArcID::TexMustache, ci.beard.mustache, t)) {
        float col[3];
        toF(getBeardColor(ci.beard.color), col);
        drawPart(cv, t, 32 * unit, mustYp * unit, mustW, mustH, 0, OriginRight, TintMode::Intensity, col, col, col);
        drawPart(cv, t, 32 * unit, mustYp * unit, mustW, mustH, 0, OriginLeft, TintMode::Intensity, col, col, col);
    }

    if (decodePartTex(ArcID::TexMouth, ci.mouth.type, t)) {
        float c0[3], c1[3];
        toF(kMouthColor0[ci.mouth.color < 3 ? ci.mouth.color : 0], c0);
        toF(kMouthColor1[ci.mouth.color < 3 ? ci.mouth.color : 0], c1);
        drawPart(cv, t, 32 * unit, mouthYp * unit, mouthW, mouthH, 0, OriginCenter, TintMode::Channels, c0, c1, white);
    }

    if (decodePartTex(ArcID::TexEyebrow, ci.eyebrow.type, t)) {
        float col[3];
        toF(getHairColor(ci.eyebrow.color), col);
        drawPart(cv, t, unit * (32.0f - browX), browYp * unit, browW, browH, browA, OriginRight, TintMode::Intensity, col, col, col);
        drawPart(cv, t, unit * (32.0f + browX), browYp * unit, browW, browH, 360.0f - browA, OriginLeft, TintMode::Intensity, col, col, col);
    }

    if (decodePartTex(ArcID::TexEye, eyeType, t)) {
        float c0[3], c1[3];
        GXColor g0 = eyeType == 9 ? GXColor{255, 130, 0, 255} : (eyeType == 20 ? GXColor{0, 255, 255, 255} : GXColor{0, 0, 0, 255});
        toF(g0, c0);
        toF(kEyeColor1[ci.eye.color < 6 ? ci.eye.color : 0], c1);
        drawPart(cv, t, unit * (32.0f - eyeX), eyeYp * unit, eyeW, eyeH, eyeA, OriginRight, TintMode::Channels, c0, c1, white);
        drawPart(cv, t, unit * (32.0f + eyeX), eyeYp * unit, eyeW, eyeH, 360.0f - eyeA, OriginLeft, TintMode::Channels, c0, c1, white);
    }

    if (ci.mole.type && decodePartTex(ArcID::TexMole, 0, t)) {
        float col[3];
        toF(kMoleColor, col);
        drawPart(cv, t, moleX * unit, moleY * unit, moleSz, moleSz, 0, OriginCenter, TintMode::Intensity, col, col, col);
    }

    for (int by = 0; by < res; by += 4) {
        for (int bx = 0; bx < res; bx += 4) {
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    const Rgba& c = cv.px[(by + y) * res + bx + x];
                    u16 v;
                    auto q = [](float f, int n) { return (u16)std::lround(std::min(1.0f, std::max(0.0f, f)) * n); };
                    if (c.a >= 0.97f) {
                        v = 0x8000 | (q(c.r, 31) << 10) | (q(c.g, 31) << 5) | q(c.b, 31);
                    } else {
                        v = (q(c.a, 7) << 12) | (q(c.r, 15) << 8) | (q(c.g, 15) << 4) | q(c.b, 15);
                    }
                    dst[0] = v >> 8;
                    dst[1] = v & 0xFF;
                    dst += 2;
                }
            }
        }
    }
}

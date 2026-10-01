#pragma once

#include "RFL_Types.h"

#if DOLPHIN_INCLUDES
#include <dolphin/mtx.h>
#include <dolphin/types.h>
#else
#include <revolution/mtx.h>
#include <revolution/types.h>
#endif

#define VTX_COORDS_IN_POS 3
#define VTX_COORDS_IN_NRM 3
#define VTX_COORDS_IN_TXC 2

#define VTX_COORD_SIZE sizeof(s16)
#define VTX_POS_SIZE (VTX_COORD_SIZE * VTX_COORDS_IN_POS)
#define VTX_NRM_SIZE (VTX_COORD_SIZE * VTX_COORDS_IN_NRM)
#define VTX_TXC_SIZE (VTX_COORD_SIZE * VTX_COORDS_IN_TXC)

#define SIZE_VTX_POS(count) ((count) * VTX_POS_SIZE)
#define SIZE_VTX_NRM(count) ((count) * VTX_NRM_SIZE)
#define SIZE_VTX_TXC(count) ((count) * VTX_TXC_SIZE)

namespace rvlfacelib {

enum class PartsShp : u32 {
    Nose = 0,
    Forehead,
    Faceline,
    Hair,
    Cap,
    Beard,
    Noseline,
    Mask,
    Glass,
    Max
};

enum class PartsShpTex : u32 {
    Face = 0,
    Cap,
    Noseline,
    Glass,
    Max
};

struct ShapeRes {
    PartsShp part;
    u16 file;
    BOOL transform;
    BOOL flipX;
    s16* vtxPosBuf;
    s16* vtxNrmBuf;
    s16* vtxTxcBuf;
    u8* dlBuf;
    u16 vtxPosBufSize;
    u16 vtxNrmBufSize;
    u16 vtxTxcBufSize;
    u16 dlBufSize;
    u16 numVtxPos;
    u16 numVtxNrm;
    u16 numVtxTxc;
    u16 dlSize;
    f32 posScale;
    Vec* posTrans;
    Vec* noseTrans;
    Vec* beardTrans;
    Vec* hairTrans;
};

struct Texture {
    u8 format;
    u8 alpha;
    u16 width;
    u16 height;
    u8 wrapS;
    u8 wrapT;
    u8 indexTexture;
    u8 colorFormat;
    u16 numColors;
    u32 paletteOfs;
    u8 enableLOD;
    u8 enableEdgeLOD;
    u8 enableBiasClamp;
    u8 enableMaxAniso;
    u8 minFilt;
    u8 magFilt;
    s8 minLOD;
    s8 maxLOD;
    u8 mipmapLevel;
    s8 reserved;
    s16 lodBias;
    u32 imageOfs;
};

inline void* getTexImage(const Texture* tex) {
    return reinterpret_cast<u8*>(const_cast<Texture*>(tex)) + tex->imageOfs;
}

struct CharInfo {
    u16 facelineType : 3;
    u16 facelineColor : 3;
    u16 facelineTexture : 4;
    u16 facelinePadding : 6;

    u16 hairType : 7;
    u16 hairColor : 3;
    u16 hairFlip : 1;
    u16 hairPadding : 5;

    u32 eyeType : 6;
    u32 eyeColor : 3;
    u32 eyeScale : 4;
    u32 eyeRotate : 5;
    u32 eyeX : 4;
    u32 eyeY : 5;
    u32 eyePadding : 5;

    u32 eyebrowType : 5;
    u32 eyebrowColor : 3;
    u32 eyebrowScale : 4;
    u32 eyebrowRotate : 5;
    u32 eyebrowX : 4;
    u32 eyebrowY : 5;
    u32 eyebrowPadding : 6;

    u16 noseType : 4;
    u16 noseScale : 4;
    u16 noseY : 5;
    u16 nosePadding : 3;

    u16 mouthType : 5;
    u16 mouthColor : 2;
    u16 mouthScale : 4;
    u16 mouthY : 5;

    u16 beardMustache : 2;
    u16 beardType : 2;
    u16 beardColor : 3;
    u16 beardScale : 4;
    u16 beardY : 5;

    u16 glassType : 4;
    u16 glassColor : 3;
    u16 glassScale : 4;
    u16 glassY : 5;

    u16 moleType : 1;
    u16 moleScale : 4;
    u16 moleX : 5;
    u16 moleY : 5;
    u16 molePadding : 1;

    u8 height;
    u8 build;
    u8 color;
    u8 personalColor;
};

}

#define NUM_VTX_POS(size) ((size) / VTX_POS_SIZE)
#define NUM_VTX_NRM(size) ((size) / VTX_NRM_SIZE)
#define NUM_VTX_TXC(size) ((size) / VTX_TXC_SIZE)

extern "C" {
    void RFLiInitShapeRes(rvlfacelib::ShapeRes* shape);
    void RFLiInitTexRes(GXTexObj* texObj, u32 part, u16 file, void* buffer);
    void RFLiInitCharModelRes(void* res, const rvlfacelib::CharInfo* info);
    u32 RFLiGetShpTexSize(u32 part, u16 file);
    void* RFLiLoadShpTexture(u32 part, u16 file, void* dest);
    void* RFLiGetTexImage(rvlfacelib::Texture* tex);
}

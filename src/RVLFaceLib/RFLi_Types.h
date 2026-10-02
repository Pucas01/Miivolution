#ifndef RVL_FACE_LIBRARY_INTERNAL_TYPES_HPP
#define RVL_FACE_LIBRARY_INTERNAL_TYPES_HPP

#include "RFL_Types.h"

#if DOLPHIN_INCLUDES
#include <dolphin/types.h>
#else
#include <revolution/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 2)

typedef struct RFLiCharInfo {
    union {
        struct {
            u16 type : 3;
            u16 color : 3;
            u16 texture : 4;
            u16 padding : 6;
        };
        u16 rawdata;
    } faceline;

    union {
        struct {
            u16 type : 7;
            u16 color : 3;
            u16 flip : 1;
            u16 padding : 5;
        };
        u16 rawdata;
    } hair;

    union {
        struct {
            u32 type : 6;
            u32 color : 3;
            u32 scale : 4;
            u32 rotate : 5;
            u32 x : 4;
            u32 y : 5;
            u32 padding : 5;
        };
        u32 rawdata;
    } eye;

    union {
        struct {
            u32 type : 5;
            u32 color : 3;
            u32 scale : 4;
            u32 rotate : 5;
            u32 x : 4;
            u32 y : 5;
            u32 padding : 6;
        };
        u32 rawdata;
    } eyebrow;

    union {
        struct {
            u16 type : 4;
            u16 scale : 4;
            u16 y : 5;
            u16 padding : 3;
        };
        u16 rawdata;
    } nose;

    union {
        struct {
            u16 type : 5;
            u16 color : 2;
            u16 scale : 4;
            u16 y : 5;
        };
        u16 rawdata;
    } mouth;

    union {
        struct {
            u16 mustache : 2;
            u16 type : 2;
            u16 color : 3;
            u16 scale : 4;
            u16 y : 5;
        };
        u16 rawdata;
    } beard;

    union {
        struct {
            u16 type : 4;
            u16 color : 3;
            u16 scale : 4;
            u16 y : 5;
        };
        u16 rawdata;
    } glass;

    union {
        struct {
            u16 type : 1;
            u16 scale : 4;
            u16 x : 5;
            u16 y : 5;
            u16 padding : 1;
        };
        u16 rawdata;
    } mole;

    struct {
        u8 height;
        u8 build;
    } body;

    struct {
        u16 name[RFL_NAME_LEN + 1];
        u16 creator[RFL_CREATOR_LEN + 1];

        u16 sex : 1;
        u16 bmonth : 4;
        u16 bday : 5;
        u16 color : 4;
        u16 favorite : 1;
        u16 localOnly : 1;
    } personal;

    RFLCreateID createID;
} RFLiCharInfo;

typedef struct RFLiCharData {
    u16 padding0 : 1;
    u16 sex : 1;
    u16 birthMonth : 4;
    u16 birthDay : 5;
    u16 favoriteColor : 4;
    u16 favorite : 1;

    u16 name[RFL_NAME_LEN];
    u8 height;
    u8 build;
    RFLCreateID createID;

    u16 faceType : 3;
    u16 faceColor : 3;
    u16 faceTex : 4;
    u16 padding2 : 3;
    u16 localonly : 1;
    u16 type : 2;

    u16 hairType : 7;
    u16 hairColor : 3;
    u16 hairFlip : 1;
    u16 padding3 : 5;

    u16 eyebrowType : 5;
    u16 eyebrowRotate : 5;
    u16 padding4 : 6;

    u16 eyebrowColor : 3;
    u16 eyebrowScale : 4;
    u16 eyebrowY : 5;
    u16 eyebrowX : 4;

    u16 eyeType : 6;
    u16 eyeRotate : 5;
    u16 eyeY : 5;

    u16 eyeColor : 3;
    u16 eyeScale : 4;
    u16 eyeX : 4;
    u16 padding5 : 5;

    u16 noseType : 4;
    u16 noseScale : 4;
    u16 noseY : 5;
    u16 padding6 : 3;

    u16 mouthType : 5;
    u16 mouthColor : 2;
    u16 mouthScale : 4;
    u16 mouthY : 5;

    u16 glassType : 4;
    u16 glassColor : 3;
    u16 glassScale : 4;
    u16 glassY : 5;

    u16 mustacheType : 2;
    u16 beardType : 2;
    u16 beardColor : 3;
    u16 beardScale : 4;
    u16 beardY : 5;

    u16 moleType : 1;
    u16 moleScale : 4;
    u16 moleY : 5;
    u16 moleX : 5;
    u16 padding8 : 1;

    u16 creatorName[RFL_CREATOR_LEN];
} RFLiCharData;

#pragma pack(pop)

void RFLiConvertRaw2Info(const RFLiCharData* data, RFLiCharInfo* out);
void RFLiGetDefaultData(RFLiCharInfo* info, u16 index);

constexpr u16 RFLiPlaceholderOfficialIndex = 0;

typedef struct RFLMiddleDB RFLMiddleDB;
RFLErrcode RFLiPickupCharInfo(void* info, RFLDataSource source, RFLMiddleDB* db, u16 index);

#ifdef __cplusplus
}
#endif

#endif // RVL_FACE_LIBRARY_INTERNAL_TYPES_HPP

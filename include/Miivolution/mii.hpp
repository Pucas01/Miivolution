#pragma once

#ifndef MIIVOLUTION_MII_HPP
#define MIIVOLUTION_MII_HPP

#if DOLPHIN_INCLUDES
#include <dolphin/types.h>
#else
#include <revolution/types.h>
#endif

#include "RFL_Types.h"

namespace miivolution::mii {

#pragma pack(push, 2)

struct MII_DATA_STRUCT {
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
};

#pragma pack(pop)

bool serializeMii(const MII_DATA_STRUCT& m, u8* out, u32 outSize);
bool deserializeMii(const u8* in, u32 inSize, MII_DATA_STRUCT& m);

bool getGuestMii(u16 index, MII_DATA_STRUCT& out);

}

#endif // MIIVOLUTION_MII_HPP

#include "RVLFaceLib/RFLi_Types.hpp"
#include <cstring>

extern "C" {

void RFLiConvertRaw2Info(const RFLiCharData* data, RFLiCharInfo* info) {
    info->faceline.type = data->faceType;
    info->faceline.color = data->faceColor;
    info->faceline.texture = data->faceTex;

    info->hair.type = data->hairType;
    info->hair.color = data->hairColor;
    info->hair.flip = data->hairFlip;

    info->eye.type = data->eyeType;
    info->eye.color = data->eyeColor;
    info->eye.scale = data->eyeScale;
    info->eye.rotate = data->eyeRotate;
    info->eye.x = data->eyeX;
    info->eye.y = data->eyeY;

    info->eyebrow.type = data->eyebrowType;
    info->eyebrow.color = data->eyebrowColor;
    info->eyebrow.scale = data->eyebrowScale;
    info->eyebrow.rotate = data->eyebrowRotate;
    info->eyebrow.x = data->eyebrowX;
    info->eyebrow.y = data->eyebrowY;

    info->nose.type = data->noseType;
    info->nose.scale = data->noseScale;
    info->nose.y = data->noseY;

    info->mouth.type = data->mouthType;
    info->mouth.color = data->mouthColor;
    info->mouth.scale = data->mouthScale;
    info->mouth.y = data->mouthY;

    info->beard.mustache = data->mustacheType;
    info->beard.type = data->beardType;
    info->beard.color = data->beardColor;
    info->beard.scale = data->beardScale;
    info->beard.y = data->beardY;

    info->glass.type = data->glassType;
    info->glass.color = data->glassColor;
    info->glass.scale = data->glassScale;
    info->glass.y = data->glassY;

    info->mole.type = data->moleType;
    info->mole.scale = data->moleScale;
    info->mole.x = data->moleX;
    info->mole.y = data->moleY;

    info->body.height = data->height;
    info->body.build = data->build;

    std::memcpy(info->personal.name, data->name, RFL_NAME_LEN * sizeof(u16));
    info->personal.name[RFL_NAME_LEN] = 0;

    std::memcpy(info->personal.creator, data->creatorName, RFL_CREATOR_LEN * sizeof(u16));
    info->personal.creator[RFL_CREATOR_LEN] = 0;

    std::memcpy(&info->createID, &data->createID, sizeof(RFLCreateID));

    info->personal.sex = data->sex;
    info->personal.bmonth = data->birthMonth;
    info->personal.bday = data->birthDay;
    info->personal.color = data->favoriteColor;
    info->personal.favorite = data->favorite;
    info->personal.localOnly = data->localonly;
}

}

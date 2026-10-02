#include "Miivolution/mii.hpp"

#include "Miivolution/database.hpp"
#include "Miivolution/storage.hpp"
#include "RVLFaceLib/RFLi_Types.h"
#include "RevoInternal/bitstream.hpp"
#include "RevoInternal/util.hpp"

#define MII_RAW_SIZE 0x4A

static_assert(sizeof(miivolution::mii::MII_DATA_STRUCT) == sizeof(RFLiCharData),
    "MII_DATA_STRUCT must exactly match RFLiCharData layout");

extern "C" {
    void RFLiGetDefaultRawData(RFLiCharData* data, u16 index);
}

namespace miivolution::mii {
    bool serializeMii(const MII_DATA_STRUCT& m, u8* out, u32 outSize) {
        if (!out || outSize < MII_RAW_SIZE) return false;

        for (u32 i = 0; i < MII_RAW_SIZE; ++i) out[i] = 0;
        revointernal::BitWriter w(out);

        w.put(m.padding0, 1);
        w.put(m.sex, 1);
        w.put(m.birthMonth, 4);
        w.put(m.birthDay, 5);
        w.put(m.favoriteColor, 4);
        w.put(m.favorite, 1);

        for (const unsigned short i : m.name) w.put(i, 16);

        w.put(m.height, 8);
        w.put(m.build, 8);

        for (int i = 0; i < 8; ++i) w.put(m.createID.data[i], 8);

        w.put(m.faceType, 3);
        w.put(m.faceColor, 3);
        w.put(m.faceTex, 4);
        w.put(m.padding2, 3);
        w.put(m.localonly, 1);
        w.put(m.type, 2);

        w.put(m.hairType, 7);
        w.put(m.hairColor, 3);
        w.put(m.hairFlip, 1);
        w.put(m.padding3, 5);

        w.put(m.eyebrowType, 5);
        w.put(m.eyebrowRotate, 5);
        w.put(m.padding4, 6);

        w.put(m.eyebrowColor, 3);
        w.put(m.eyebrowScale, 4);
        w.put(m.eyebrowY, 5);
        w.put(m.eyebrowX, 4);

        w.put(m.eyeType, 6);
        w.put(m.eyeRotate, 5);
        w.put(m.eyeY, 5);

        w.put(m.eyeColor, 3);
        w.put(m.eyeScale, 4);
        w.put(m.eyeX, 4);
        w.put(m.padding5, 5);

        w.put(m.noseType, 4);
        w.put(m.noseScale, 4);
        w.put(m.noseY, 5);
        w.put(m.padding6, 3);

        w.put(m.mouthType, 5);
        w.put(m.mouthColor, 2);
        w.put(m.mouthScale, 4);
        w.put(m.mouthY, 5);

        w.put(m.glassType, 4);
        w.put(m.glassColor, 3);
        w.put(m.glassScale, 4);
        w.put(m.glassY, 5);

        w.put(m.mustacheType, 2);
        w.put(m.beardType, 2);
        w.put(m.beardColor, 3);
        w.put(m.beardScale, 4);
        w.put(m.beardY, 5);

        w.put(m.moleType, 1);
        w.put(m.moleScale, 4);
        w.put(m.moleY, 5);
        w.put(m.moleX, 5);
        w.put(m.padding8, 1);

        for (const unsigned short i : m.creatorName) w.put(i, 16);

        return true;
    }

    bool deserializeMii(const u8* in, const u32 inSize, MII_DATA_STRUCT& m) {
        if (!in || inSize < MII_RAW_SIZE) return false;

        revointernal::BitReader r(in);

        m.padding0 = r.get(1);
        m.sex = r.get(1);
        m.birthMonth = r.get(4);
        m.birthDay = r.get(5);
        m.favoriteColor = r.get(4);
        m.favorite = r.get(1);

        for (unsigned short & i : m.name) i = static_cast<u16>(r.get(16));

        m.height = static_cast<u8>(r.get(8));
        m.build = static_cast<u8>(r.get(8));

        for (int i = 0; i < 8; ++i) m.createID.data[i] = static_cast<u8>(r.get(8));

        m.faceType = r.get(3);
        m.faceColor = r.get(3);
        m.faceTex = r.get(4);
        m.padding2 = r.get(3);
        m.localonly = r.get(1);
        m.type = r.get(2);

        m.hairType = r.get(7);
        m.hairColor = r.get(3);
        m.hairFlip = r.get(1);
        m.padding3 = r.get(5);

        m.eyebrowType = r.get(5);
        m.eyebrowRotate = r.get(5);
        m.padding4 = r.get(6);

        m.eyebrowColor = r.get(3);
        m.eyebrowScale = r.get(4);
        m.eyebrowY = r.get(5);
        m.eyebrowX = r.get(4);

        m.eyeType = r.get(6);
        m.eyeRotate = r.get(5);
        m.eyeY = r.get(5);

        m.eyeColor = r.get(3);
        m.eyeScale = r.get(4);
        m.eyeX = r.get(4);
        m.padding5 = r.get(5);

        m.noseType = r.get(4);
        m.noseScale = r.get(4);
        m.noseY = r.get(5);
        m.padding6 = r.get(3);

        m.mouthType = r.get(5);
        m.mouthColor = r.get(2);
        m.mouthScale = r.get(4);
        m.mouthY = r.get(5);

        m.glassType = r.get(4);
        m.glassColor = r.get(3);
        m.glassScale = r.get(4);
        m.glassY = r.get(5);

        m.mustacheType = r.get(2);
        m.beardType = r.get(2);
        m.beardColor = r.get(3);
        m.beardScale = r.get(4);
        m.beardY = r.get(5);

        m.moleType = r.get(1);
        m.moleScale = r.get(4);
        m.moleY = r.get(5);
        m.moleX = r.get(5);
        m.padding8 = r.get(1);

        for (unsigned short & i : m.creatorName) i = static_cast<u16>(r.get(16));

        return true;
    }

    bool getGuestMii(u16 index, MII_DATA_STRUCT& out) {
        if (index >= 6) return false;

        RFLiCharData* data = reinterpret_cast<RFLiCharData*>(&out);
        RFLiGetDefaultRawData(data, index);

        return true;
    }

    void init(const database::PrefPathConfig &cfg) {
        database::setPrefPath(cfg);

        // create import / export dirs if they don't exist
        const auto prefDir = miivolution::util::getPrefDir();
        std::filesystem::create_directories(prefDir / "miimports");
        std::filesystem::create_directories(prefDir / "miixports");

        // import all miis from the import directory
        miivolution::storage::importAllMiis();
    }
}

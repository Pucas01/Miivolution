#include "Miivolution/mii.hpp"

#define MII_RAW_SIZE 0x4A

static_assert(sizeof(miivolution::mii::MII_DATA_STRUCT) >= MII_RAW_SIZE, "Mii struct too small");

namespace {
class BitWriter {
public:
    explicit BitWriter(u8* buf) : mBuf(buf), mPos(0) {}

    void put(const u32 value, const u32 bits) {
        for (u32 i = bits; i > 0; --i) {
            const u32 bit = (value >> (i - 1)) & 1;
            mBuf[mPos >> 3] |= static_cast<u8>(bit << (7 - (mPos & 7)));
            ++mPos;
        }
    }

private:
    u8* mBuf;
    u32 mPos;
};

class BitReader {
public:
    explicit BitReader(const u8* buf) : mBuf(buf), mPos(0) {}

    u32 get(const u32 bits) {
        u32 value = 0;
        for (u32 i = 0; i < bits; ++i) {
            const u32 bit = (mBuf[mPos >> 3] >> (7 - (mPos & 7))) & 1;
            value = (value << 1) | bit;
            ++mPos;
        }
        return value;
    }

private:
    const u8* mBuf;
    u32 mPos;
};

} // namespace

namespace miivolution::mii {
    bool serializeMii(const MII_DATA_STRUCT& m, u8* out, u32 outSize) {
        if (!out || outSize < MII_RAW_SIZE) return false;

        for (u32 i = 0; i < MII_RAW_SIZE; ++i) out[i] = 0;
        BitWriter w(out);

        w.put(m.invalid, 1);
        w.put(m.isGirl, 1);
        w.put(m.month, 4);
        w.put(m.day, 5);
        w.put(m.favColor, 4);
        w.put(m.isFavorite, 1);

        for (const unsigned short i : m.name) w.put(i, 16);

        w.put(m.height, 8);
        w.put(m.weight, 8);

        w.put(m.miiID1, 8);
        w.put(m.miiID2, 8);
        w.put(m.miiID3, 8);
        w.put(m.miiID4, 8);

        w.put(m.systemID0, 8);
        w.put(m.systemID1, 8);
        w.put(m.systemID2, 8);
        w.put(m.systemID3, 8);

        w.put(m.faceShape, 3);
        w.put(m.skinColor, 3);
        w.put(m.facialFeature, 4);
        w.put(m.unknown1, 3);
        w.put(m.mingleOff, 1);
        w.put(m.unknown2, 1);
        w.put(m.downloaded, 1);

        w.put(m.hairType, 7);
        w.put(m.hairColor, 3);
        w.put(m.hairPart, 1);
        w.put(m.unknown3, 5);

        w.put(m.eyebrowType, 5);
        w.put(m.unknown4, 1);
        w.put(m.eyebrowRotation, 4);
        w.put(m.unknown5, 6);
        w.put(m.eyebrowColor, 3);
        w.put(m.eyebrowSize, 4);
        w.put(m.eyebrowVertPos, 5);
        w.put(m.eyebrowHorizSpacing, 4);

        w.put(m.eyeType, 6);
        w.put(m.unknown6, 2);
        w.put(m.eyeRotation, 3);
        w.put(m.eyeVertPos, 5);
        w.put(m.eyeColor, 3);
        w.put(m.unknown7, 1);
        w.put(m.eyeSize, 3);
        w.put(m.eyeHorizSpacing, 4);
        w.put(m.unknown8, 5);

        w.put(m.noseType, 4);
        w.put(m.noseSize, 4);
        w.put(m.noseVertPos, 5);
        w.put(m.unknown9, 3);

        w.put(m.lipType, 5);
        w.put(m.lipColor, 2);
        w.put(m.lipSize, 4);
        w.put(m.lipVertPos, 5);

        w.put(m.glassesType, 4);
        w.put(m.glassesColor, 3);
        w.put(m.unknown10, 1);
        w.put(m.glassesSize, 3);
        w.put(m.glassesVertPos, 5);

        w.put(m.mustacheType, 2);
        w.put(m.beardType, 2);
        w.put(m.facialHairColor, 3);
        w.put(m.mustacheSize, 4);
        w.put(m.mustacheVertPos, 5);

        w.put(m.moleOn, 1);
        w.put(m.moleSize, 4);
        w.put(m.moleVertPos, 5);
        w.put(m.moleHorizPos, 5);
        w.put(m.unknown11, 1);

        for (const unsigned short i : m.creatorName) w.put(i, 16);

        return true;
    }

    bool deserializeMii(const u8* in, const u32 inSize, MII_DATA_STRUCT& m) {
        if (!in || inSize < MII_RAW_SIZE) return false;

        BitReader r(in);

        m.invalid        = r.get(1);
        m.isGirl         = r.get(1);
        m.month          = r.get(4);
        m.day            = r.get(5);
        m.favColor       = r.get(4);
        m.isFavorite     = r.get(1);

        for (unsigned short & i : m.name) i = static_cast<u16>(r.get(16));

        m.height         = static_cast<u8>(r.get(8));
        m.weight         = static_cast<u8>(r.get(8));

        m.miiID1         = static_cast<u8>(r.get(8));
        m.miiID2         = static_cast<u8>(r.get(8));
        m.miiID3         = static_cast<u8>(r.get(8));
        m.miiID4         = static_cast<u8>(r.get(8));

        m.systemID0      = static_cast<u8>(r.get(8));
        m.systemID1      = static_cast<u8>(r.get(8));
        m.systemID2      = static_cast<u8>(r.get(8));
        m.systemID3      = static_cast<u8>(r.get(8));

        m.faceShape      = r.get(3);
        m.skinColor      = r.get(3);
        m.facialFeature  = r.get(4);
        m.unknown1       = r.get(3);
        m.mingleOff      = r.get(1);
        m.unknown2       = r.get(1);
        m.downloaded     = r.get(1);

        m.hairType       = r.get(7);
        m.hairColor      = r.get(3);
        m.hairPart       = r.get(1);
        m.unknown3       = r.get(5);

        m.eyebrowType         = r.get(5);
        m.unknown4            = r.get(1);
        m.eyebrowRotation     = r.get(4);
        m.unknown5            = r.get(6);
        m.eyebrowColor        = r.get(3);
        m.eyebrowSize         = r.get(4);
        m.eyebrowVertPos      = r.get(5);
        m.eyebrowHorizSpacing = r.get(4);

        m.eyeType        = r.get(6);
        m.unknown6       = r.get(2);
        m.eyeRotation    = r.get(3);
        m.eyeVertPos     = r.get(5);
        m.eyeColor       = r.get(3);
        m.unknown7       = r.get(1);
        m.eyeSize        = r.get(3);
        m.eyeHorizSpacing= r.get(4);
        m.unknown8       = r.get(5);

        m.noseType       = r.get(4);
        m.noseSize       = r.get(4);
        m.noseVertPos    = r.get(5);
        m.unknown9       = r.get(3);

        m.lipType        = r.get(5);
        m.lipColor       = r.get(2);
        m.lipSize        = r.get(4);
        m.lipVertPos     = r.get(5);

        m.glassesType    = r.get(4);
        m.glassesColor   = r.get(3);
        m.unknown10      = r.get(1);
        m.glassesSize    = r.get(3);
        m.glassesVertPos = r.get(5);

        m.mustacheType   = r.get(2);
        m.beardType      = r.get(2);
        m.facialHairColor= r.get(3);
        m.mustacheSize   = r.get(4);
        m.mustacheVertPos= r.get(5);

        m.moleOn         = r.get(1);
        m.moleSize       = r.get(4);
        m.moleVertPos    = r.get(5);
        m.moleHorizPos   = r.get(5);
        m.unknown11      = r.get(1);

        for (unsigned short & i : m.creatorName) i = static_cast<u16>(r.get(16));

        return true;
    }
}

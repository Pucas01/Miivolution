#ifndef REVO_INTERNAL_BITSTREAM_HPP
#define REVO_INTERNAL_BITSTREAM_HPP

#if DOLPHIN_INCLUDES
#include <dolphin/types.h>
#else
#include <revolution/types.h>
#endif

namespace revointernal {

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

} // namespace revointernal

#endif // REVO_INTERNAL_BITSTREAM_HPP
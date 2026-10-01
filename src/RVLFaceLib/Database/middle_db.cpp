#include "RFL_MiddleDatabase.h"
#include "RVLFaceLib/internal.hpp"
#include <cstring>

namespace {

constexpr u32 HIDDEN_CHAR_DATA_SIZE = 0x60;

}

extern "C" {

u32 RFLGetMiddleDBBufferSize(u16 size) {
    return size * HIDDEN_CHAR_DATA_SIZE;
}

void RFLInitMiddleDB(RFLMiddleDB* db, RFLMiddleDBType type, void* buffer, u16 size) {
    if (!db || !buffer) {
        return;
    }

    std::memset(db, 0, sizeof(RFLMiddleDB));
    std::memset(buffer, 0, RFLGetMiddleDBBufferSize(size));
}

}

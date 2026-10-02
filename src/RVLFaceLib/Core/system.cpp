#include "RFL_System.h"
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <filesystem>

#include "RVLFaceLib/RFLi_Database.h"
#include "RVLFaceLib/internal.hpp"
#include "Miivolution/storage.hpp"
#include "RevoInternal/util.hpp"

#ifndef _WIN32
#include <cstdlib>
#endif

extern "C" void RFLiInitResourceCache(void* data, u32 size);
extern "C" RFLiDatabase* RFLiGetDatabase();

namespace {

struct SystemState {
    bool initialized = false;
    void* workBuffer = nullptr;
    void* resBuffer = nullptr;
    u32 resSize = 0;
    bool deluxeTex = false;
    RFLErrcode asyncStatus = RFLErrcode_Success;
    s32 lastReason = 0;

    void reset() {
        initialized = false;
        workBuffer = nullptr;
        resBuffer = nullptr;
        resSize = 0;
        deluxeTex = false;
        asyncStatus = RFLErrcode_Success;
        lastReason = 0;
    }
};

constexpr u32 RFL_WORK_SIZE = 0x4B000;
constexpr u32 RFL_DELUXE_WORK_SIZE = 0x64000;

SystemState systemState;

}

extern "C" {

void* RFLiAlloc32(u32 size) {
    void* ptr = nullptr;
    #ifdef _WIN32
        ptr = _aligned_malloc(size, 32);
    #else
        if (posix_memalign(&ptr, 32, size) != 0) {
            ptr = nullptr;
        }
    #endif
    return ptr;
}

void RFLiFree(void* block) {
    if (!block) return;

    #ifdef _WIN32
        _aligned_free(block);
    #else
        free(block);
    #endif
}

BOOL RFLiGetUseDeluxTex() {
    return systemState.deluxeTex;
}

u32 RFLGetWorkSize(BOOL deluxeTex) {
    return deluxeTex ? RFL_DELUXE_WORK_SIZE : RFL_WORK_SIZE;
}

RFLErrcode RFLInitResAsync(void* workBuffer, void* resBuffer, u32 resSize, BOOL deluxeTex) {

    if (!workBuffer || !resBuffer || resSize == 0) {
        return RFLErrcode_WrongParam;
    }

    auto& state = systemState;

    if (state.initialized) {
        return RFLErrcode_Success;
    }

    u32 requiredWorkSize = RFLGetWorkSize(deluxeTex);
    std::memset(workBuffer, 0, requiredWorkSize);

    state.workBuffer = workBuffer;
    state.resBuffer = resBuffer;
    state.resSize = resSize;
    state.deluxeTex = deluxeTex;
    state.asyncStatus = RFLErrcode_Success;
    state.lastReason = 0;
    state.initialized = true;

    RFLiInitResourceCache(resBuffer, resSize);

    // Trigger database load (mimics RFLiBootLoadDatabaseAsync behavior)
    RFLiGetDatabase();

    return RFLErrcode_Success;
}

RFLErrcode RFLInitRes(void* workBuffer, void* resBuffer, u32 resSize, BOOL deluxeTex) {
    if (!workBuffer || !resBuffer || resSize == 0) {
        return RFLErrcode_WrongParam;
    }

    RFLErrcode result = RFLInitResAsync(workBuffer, resBuffer, resSize, deluxeTex);
    if (result == RFLErrcode_Success || result == RFLErrcode_Busy) {
        result = RFLWaitAsync();
    }
    return result;
}

void RFLExit(void) {
    systemState.reset();
}

BOOL RFLAvailable(void) {
    return systemState.initialized ? TRUE : FALSE;
}

RFLErrcode RFLGetAsyncStatus(void) {
    return systemState.asyncStatus;
}

s32 RFLGetLastReason(void) {
    return systemState.lastReason;
}

RFLErrcode RFLWaitAsync(void) {
    return systemState.asyncStatus;
}

}

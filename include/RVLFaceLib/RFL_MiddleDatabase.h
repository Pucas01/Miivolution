#ifndef RVL_FACE_LIBRARY_MIDDLE_DATABASE_H
#define RVL_FACE_LIBRARY_MIDDLE_DATABASE_H

#include "RFL_Types.h"

#if DOLPHIN_INCLUDES
#include <dolphin/types.h>
#else
#include <revolution/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RFLMiddleDBType_HiddenRandom,
    RFLMiddleDBType_HiddenNewer,
    RFLMiddleDBType_HiddenOlder,
    RFLMiddleDBType_Random,
    RFLMiddleDBType_UserSet,
    RFLMiddleDBType_Reserved1
} RFLMiddleDBType;

typedef struct RFLMiddleDB {
    u8 dummy[0x18];
} RFLMiddleDB;

u32 RFLGetMiddleDBBufferSize(u16 size);
void RFLInitMiddleDB(RFLMiddleDB* db, RFLMiddleDBType type, void* buffer,
                     u16 size);

#ifdef __cplusplus
}
#endif

#endif // RVL_FACE_LIBRARY_MIDDLE_DATABASE_H

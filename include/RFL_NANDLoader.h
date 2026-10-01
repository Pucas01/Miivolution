#ifndef RVL_FACE_LIBRARY_NAND_LOADER_H
#define RVL_FACE_LIBRARY_NAND_LOADER_H

#include "RFL_Types.h"

#if DOLPHIN_INCLUDES
#include <dolphin/types.h>
#else
#include <revolution/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

BOOL RFLFreeCachedResource(void);
BOOL RFLIsResourceCached(void);

#ifdef __cplusplus
}
#endif

#endif // RVL_FACE_LIBRARY_NAND_LOADER_H

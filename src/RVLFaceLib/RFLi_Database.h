#ifndef RVL_FACE_LIBRARY_INTERNAL_DATABASE_H
#define RVL_FACE_LIBRARY_INTERNAL_DATABASE_H

#include "RVLFaceLib/RFLi_Types.h"

#if DOLPHIN_INCLUDES
#include <dolphin/types.h>
#else
#include <revolution/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define RFL_DB_CHAR_MAX 100
#define RFLi_HDB_DATA_MAX 10000

#pragma pack(push, 2)

// Mii Parade data table entry (linked list node)
typedef struct RFLiTableData {
    RFLCreateID createID;

    u16 sex : 1;
    s16 next : 15;

    s16 padding : 1;
    s16 prev : 15;
} RFLiTableData;

// Mii Parade database (RNHD section at 0x1D00 in RFL_DB.dat)
typedef struct RFLiHiddenDB {
    u32 identifier; // RNHD magic (0x524E4844)
    s16 head;
    s16 tail;
    RFLiTableData data[RFLi_HDB_DATA_MAX];
    u8 padding[22];
    u16 crc; // CRC16 of this structure
} RFLiHiddenDB;

// Mii character data for Mii Parade (hidden database)
typedef struct RFLiHiddenCharData {
    u16 padding0 : 1;
    u16 sex : 1;
    u16 birthPadding : 9;
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
} RFLiHiddenCharData;

// Main Mii database (RFL_DB.dat file structure)
// File layout:
//   0x00000 - 0x01D00: Mii data (RNOD magic, followed by RFL_DB_CHAR_MAX entries)
//   0x01D00 - 0x1F1DE: Mii Parade data (RNHD magic, followed by linked list)
//   0x1F1DE - 0x1F1E0: CRC16 of entire file
typedef struct RFLiDatabase {
    u32 identifier; // RNOD magic (0x524E4F44)
    RFLiCharData rawData[RFL_DB_CHAR_MAX];

    u32 isolation : 1;
    u32 padding1 : 31;

    u8 specialInvite[13];
    u8 nwc24Month;
    u8 nwc24Day;
    u8 padding2;

    RFLiHiddenDB hidden; // Mii Parade data starts at 0x1D00
} RFLiDatabase;

#pragma pack(pop)

static_assert(sizeof(RFLiTableData) == 12, "RFLiTableData size mismatch");
static_assert(sizeof(RFLiHiddenDB) == 0x1D4E0, "RFLiHiddenDB size mismatch");
static_assert(sizeof(RFLiDatabase) == 0x1F1E0, "RFLiDatabase size mismatch");

#ifdef __cplusplus
}
#endif

#endif // RVL_FACE_LIBRARY_INTERNAL_DATABASE_H

#ifndef MIIVOLUTION_DB_HPP
#define MIIVOLUTION_DB_HPP

#include "mii.hpp"
#include "RFL_Types.h"
#include <filesystem>
#include <functional>

#if DOLPHIN_INCLUDES
#include <dolphin/types.h>
#else
#include <revolution/types.h>
#endif

namespace miivolution::database {

bool loadDatabase();
bool saveDatabase();
bool createEmptyDatabase();
bool isLoaded();

u32 getMiiCount();
bool getMii(u16 index, mii::MII_DATA_STRUCT& out);
bool setMii(u16 index, const mii::MII_DATA_STRUCT& data);

s32 findMiiByCreateID(const RFLCreateID& id);
s32 findEmptySlot();
bool addMii(const mii::MII_DATA_STRUCT& data, u16* outIndex = nullptr);
bool deleteMii(u16 index);

std::filesystem::path getDatabasePath();
u16 calculateCRC16(const void* data, u32 size);

using PrefPathFn = std::function<char *(const char *org, const char *app)>;
using PrefFreeFn = std::function<void(void *)>;

struct PrefPathConfig {
    PrefPathFn get;
    PrefFreeFn free;
};

void setPrefPath(PrefPathConfig cfg);
}

#endif // MIIVOLUTION_DB_HPP

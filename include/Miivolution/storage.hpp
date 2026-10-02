#pragma once

#ifndef MIIVOLUTION_STORAGE_HPP
#define MIIVOLUTION_STORAGE_HPP

#include "mii.hpp"
#include "RFL_Types.h"
#include <filesystem>
#include <optional>
#include <vector>

#if DOLPHIN_INCLUDES
#include <dolphin/types.h>
#else
#include <revolution/types.h>
#endif

namespace miivolution::storage {

bool exportMii(const mii::MII_DATA_STRUCT& mii,
               const std::optional<std::filesystem::path>& path = std::nullopt);

bool exportMii(u16 index,
               const std::optional<std::filesystem::path>& path = std::nullopt);

bool exportMii(const RFLCreateID& createID,
               const std::optional<std::filesystem::path>& path = std::nullopt);

bool importMii(const std::filesystem::path& path, mii::MII_DATA_STRUCT& out);

bool importMiiToDB(const std::filesystem::path& path, u16* outIndex = nullptr);

std::vector<std::filesystem::path> getMiixports();

std::vector<std::filesystem::path> getMiimports();

u32 importAllMiis(std::vector<u16>* outIndices = nullptr);

}

#endif // MIIVOLUTION_STORAGE_HPP

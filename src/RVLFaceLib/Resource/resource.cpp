#include "RVLFaceLib/resource.hpp"
#include "RVLFaceLib/internal.hpp"
#include "RevoInternal/endian.hpp"
#include <cstring>
#include <algorithm>

namespace rvlfacelib {

namespace {

ResourceLoader resLoader;

constexpr u32 roundUp(u32 value, u32 alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

}

void ResourceLoader::init(void* resourceData, u32 size) {
    data_ = resourceData;
    size_ = size;

    if (!data_ || size < 0x100) {
        return;
    }

    const u8* ptr = static_cast<const u8*>(data_);

    header_.version = revointernal::readBE<u16>(ptr + 2);

    for (u32 i = 0; i < static_cast<u32>(ArcID::Max); i++) {
        const u32 archiveOffset = revointernal::readBE<u32>(ptr + ((i + 1) * 4));

        if (archiveOffset >= size) {
            continue;
        }

        const u8* archivePtr = ptr + archiveOffset;
        header_.archives[i].numFiles = revointernal::readBE<u16>(archivePtr);
        header_.archives[i].biggestSize = revointernal::readBE<u16>(archivePtr + 2);
        header_.archives[i].offset = archiveOffset + 4;
    }
}

const u8* ResourceLoader::getFileData(ArcID arc, u16 fileIndex, u32* outSize) const {
    if (!isValid()) {
        return nullptr;
    }

    const u32 arcIdx = static_cast<u32>(arc);
    if (arcIdx >= static_cast<u32>(ArcID::Max)) {
        return nullptr;
    }

    const auto& archive = header_.archives[arcIdx];
    if (fileIndex >= archive.numFiles || archive.offset == 0) {
        return nullptr;
    }

    const u8* ptr = static_cast<const u8*>(data_);
    const u8* archiveData = ptr + archive.offset;

    const u32 fileOffset = revointernal::readBE<u32>(archiveData + fileIndex * 4);
    const u32 nextOffset = revointernal::readBE<u32>(archiveData + (fileIndex + 1) * 4);
    const u32 fileSize = nextOffset - fileOffset;

    if (outSize) {
        *outSize = fileSize;
    }

    return archiveData + fileOffset;
}

u32 ResourceLoader::getShapeSize(ArcID arc, u16 fileIndex) const {
    u32 size = 0;
    getFileData(arc, fileIndex, &size);
    return size;
}

void ResourceLoader::loadShape(ArcID arc, u16 fileIndex, void* dest) const {
    if (!dest) {
        return;
    }

    u32 size = 0;
    const u8* fileData = getFileData(arc, fileIndex, &size);

    if (fileData && size > 0) {
        std::memcpy(dest, fileData, size);
    }
}

u32 ResourceLoader::getTextureSize(ArcID arc, u16 fileIndex) const {
    u32 size = 0;
    getFileData(arc, fileIndex, &size);
    return size;
}

void ResourceLoader::loadTexture(ArcID arc, u16 fileIndex, void* dest) const {
    if (!dest) {
        return;
    }

    u32 size = 0;
    const u8* fileData = getFileData(arc, fileIndex, &size);

    if (fileData && size > 0) {
        std::memcpy(dest, fileData, size);
    }
}

ResourceLoader& getResourceLoader() {
    return resLoader;
}

}

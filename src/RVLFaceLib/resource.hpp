#pragma once

#include "RFL_Types.h"

namespace rvlfacelib {

enum class ArcID : u32 {
    ShapeBeard = 0,
    TexEye,
    TexEyebrow,
    ShapeFaceline,
    TexFaceline,
    ShapeForehead,
    ShapeGlass,
    TexGlass,
    ShapeHair,
    ShapeMask,
    TexMole,
    TexMouth,
    TexMustache,
    ShapeNose,
    ShapeNoseline,
    TexNoseline,
    ShapeCap,
    TexCap,
    Max
};

struct ResourceArchive {
    u16 numFiles = 0;
    u16 biggestSize = 0;
    u32 offset = 0;
};

struct ResourceHeader {
    u16 version = 0;
    ResourceArchive archives[static_cast<u32>(ArcID::Max)];
};

class ResourceLoader {
public:
    ResourceLoader() = default;

    void init(void* resourceData, u32 size);
    bool isValid() const { return data_ != nullptr && size_ > 0; }

    const u8* getFile(ArcID arc, u16 fileIndex, u32* outSize) const { return getFileData(arc, fileIndex, outSize); }
    u32 getShapeSize(ArcID arc, u16 fileIndex) const;
    void loadShape(ArcID arc, u16 fileIndex, void* dest) const;

    u32 getTextureSize(ArcID arc, u16 fileIndex) const;
    void loadTexture(ArcID arc, u16 fileIndex, void* dest) const;

private:
    const u8* getFileData(ArcID arc, u16 fileIndex, u32* outSize) const;

    void* data_ = nullptr;
    u32 size_ = 0;
    ResourceHeader header_;
};

ResourceLoader& getResourceLoader();

}

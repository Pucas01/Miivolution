#include "RVLFaceLib/internal.hpp"
#include "RVLFaceLib/resource.hpp"
#include "RVLFaceLib/RFL_Model.h"

extern "C" {

u32 RFLiGetShapeSize(u32 part, u16 file) {
    auto& loader = rvlfacelib::getResourceLoader();
    if (!loader.isValid()) {
        return 0;
    }

    auto arcId = static_cast<rvlfacelib::ArcID>(part);
    return loader.getShapeSize(arcId, file);
}

void RFLiLoadShape(u32 part, u16 file, void* dest) {
    auto& loader = rvlfacelib::getResourceLoader();
    if (!loader.isValid() || !dest) {
        return;
    }

    auto arcId = static_cast<rvlfacelib::ArcID>(part);
    loader.loadShape(arcId, file, dest);
}

u32 RFLiGetTexSize(u32 part, u16 file) {
    auto& loader = rvlfacelib::getResourceLoader();
    if (!loader.isValid()) {
        return 0;
    }

    auto arcId = static_cast<rvlfacelib::ArcID>(part);
    return loader.getTextureSize(arcId, file);
}

void RFLiLoadTexture(u32 part, u16 file, void* dest) {
    auto& loader = rvlfacelib::getResourceLoader();
    if (!loader.isValid() || !dest) {
        return;
    }

    auto arcId = static_cast<rvlfacelib::ArcID>(part);
    loader.loadTexture(arcId, file, dest);
}

u32 RFLiGetShpTexSize(u32 part, u16 file) {
    return RFLiGetTexSize(part, file);
}

void RFLiLoadShpTexture(u32 part, u16 file, void* dest) {
    RFLiLoadTexture(part, file, dest);
}

}

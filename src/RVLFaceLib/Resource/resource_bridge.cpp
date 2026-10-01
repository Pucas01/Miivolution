#include "RFL_Model.h"

#include "RVLFaceLib/internal.hpp"
#include "RVLFaceLib/resource.hpp"

namespace {
using rvlfacelib::ArcID;
ArcID shapeArc(u32 part) {
    static const ArcID map[] = {ArcID::ShapeNose, ArcID::ShapeForehead, ArcID::ShapeFaceline, ArcID::ShapeHair, ArcID::ShapeCap,
                                ArcID::ShapeBeard, ArcID::ShapeNoseline, ArcID::ShapeMask, ArcID::ShapeGlass};
    return map[part < 9 ? part : 0];
}
ArcID texArc(u32 part) {
    static const ArcID map[] = {ArcID::TexFaceline, ArcID::TexCap, ArcID::TexNoseline, ArcID::TexGlass};
    return map[part < 4 ? part : 0];
}
}

extern "C" {

u32 RFLiGetShapeSize(u32 part, u16 file) {
    auto& loader = rvlfacelib::getResourceLoader();
    if (!loader.isValid()) {
        return 0;
    }

    auto arcId = shapeArc(part);
    return loader.getShapeSize(arcId, file);
}

void RFLiLoadShape(u32 part, u16 file, void* dest) {
    auto& loader = rvlfacelib::getResourceLoader();
    if (!loader.isValid() || !dest) {
        return;
    }

    auto arcId = shapeArc(part);
    loader.loadShape(arcId, file, dest);
}

u32 RFLiGetTexSize(u32 part, u16 file) {
    auto& loader = rvlfacelib::getResourceLoader();
    if (!loader.isValid()) {
        return 0;
    }

    auto arcId = texArc(part);
    return loader.getTextureSize(arcId, file);
}

void RFLiLoadTexture(u32 part, u16 file, void* dest) {
    auto& loader = rvlfacelib::getResourceLoader();
    if (!loader.isValid() || !dest) {
        return;
    }

    auto arcId = texArc(part);
    loader.loadTexture(arcId, file, dest);
}

u32 RFLiGetShpTexSize(u32 part, u16 file) {
    return RFLiGetTexSize(part, file);
}

void RFLiLoadShpTexture(u32 part, u16 file, void* dest) {
    RFLiLoadTexture(part, file, dest);
}

}

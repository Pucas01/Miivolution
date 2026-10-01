#include "RVLFaceLib/internal.hpp"
#include "RVLFaceLib/RFL_NANDLoader.h"
#include "RVLFaceLib/RFL_System.h"
#include "RVLFaceLib/resource.hpp"

namespace {

struct ResourceCache {
    void* data = nullptr;
    u32 size = 0;
    bool cached = false;
};

ResourceCache resourceCache;

}

extern "C" {

BOOL RFLFreeCachedResource(void) {
    if (!RFLAvailable()) {
        return FALSE;
    }

    if (resourceCache.cached && resourceCache.data) {
        resourceCache.data = nullptr;
        resourceCache.size = 0;
        resourceCache.cached = false;
        return TRUE;
    }

    return FALSE;
}

BOOL RFLIsResourceCached(void) {
    if (!RFLAvailable()) {
        return FALSE;
    }

    return resourceCache.cached ? TRUE : FALSE;
}

void RFLiInitResourceCache(void* data, u32 size) {
    resourceCache.data = data;
    resourceCache.size = size;
    resourceCache.cached = (data != nullptr && size > 0);

    if (resourceCache.cached) {
        rvlfacelib::getResourceLoader().init(data, size);
    }
}

}

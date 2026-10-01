#pragma once

#include "RevoInternal/log.hpp"
#include "RVLFaceLib/RFL_Types.h"

#define RVL_NOT_IMPL(fn_name) \
    ::revointernal::log(::revointernal::LogLevel::Warning, "[RVLFaceLib] Not implemented: %s", fn_name)

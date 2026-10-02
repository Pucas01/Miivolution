#pragma once

#ifndef MIIVOLUTION_UTIL_HPP
#define MIIVOLUTION_UTIL_HPP

#include <filesystem>
#include <functional>

namespace miivolution::util {

    using PrefPathFn = std::function<std::filesystem::path()>;
    void setPrefPathFunction(PrefPathFn fn);

    std::filesystem::path getPrefDir();
    std::filesystem::path getFaceDatabase();

}

#endif // MIIVOLUTION_UTIL_HPP

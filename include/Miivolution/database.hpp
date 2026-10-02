#ifndef MIIVOLUTION_DB_HPP
#define MIIVOLUTION_DB_HPP

#include <functional>

namespace miivolution::database {

// TODO: getMiiDatabase (returns filepath OR RFL_DB struct)
// TODO: getMiiFromDatabase(some_identifier)
// TODO: saveMii (accepts MII_DATA_STRUCT or RFLiCharData)

using PrefPathFn = std::function<char *(const char *org, const char *app)>;
using PrefFreeFn = std::function<void(void *)>;

struct PrefPathConfig {
    PrefPathFn get;
    PrefFreeFn free;
};

void setPrefPath(PrefPathConfig cfg);
}

#endif // MIIVOLUTION_DB_HPP

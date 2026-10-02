#include <catch2/catch_session.hpp>
#include "Miivolution/database.hpp"
#include <cstdlib>
#include <cstring>

static char* testPrefPath(const char*, const char*) {
    const char* tmpdir = std::getenv("TMPDIR");
    if (!tmpdir) tmpdir = "/tmp";
    return strdup(tmpdir);
}

static void testFreePath(void* ptr) {
    free(ptr);
}

int main(int argc, char* argv[]) {
    miivolution::database::setPrefPath({testPrefPath, testFreePath});
    return Catch::Session().run(argc, argv);
}

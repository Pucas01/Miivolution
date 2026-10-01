set(LIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src/RVLFaceLib")

# Core

list(APPEND RVLFACE_SOURCES
    ${LIB_ROOT}/Core/system.cpp
    ${LIB_ROOT}/Core/stubs.cpp
)

# Model

list(APPEND RVLFACE_SOURCES
    ${LIB_ROOT}/Model/model.cpp
    ${LIB_ROOT}/Model/charinfo.cpp
)

# Resource

list(APPEND RVLFACE_SOURCES
    ${LIB_ROOT}/Resource/resource.cpp
    ${LIB_ROOT}/Resource/resource_bridge.cpp
    ${LIB_ROOT}/Resource/nand_loader.cpp
)

# Database

list(APPEND RVLFACE_SOURCES
    ${LIB_ROOT}/Database/database.cpp
    ${LIB_ROOT}/Database/middle_db.cpp
    ${LIB_ROOT}/Database/controller.cpp
)

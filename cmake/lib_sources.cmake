set(LIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src/RVLFaceLib")

list(APPEND REVOMII_RVLFACE_SOURCES
    ${LIB_ROOT}/controller.cpp
    ${LIB_ROOT}/database.cpp
    ${LIB_ROOT}/model.cpp
    ${LIB_ROOT}/stubs.cpp
    ${LIB_ROOT}/system.cpp
)

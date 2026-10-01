cmake_minimum_required(VERSION 3.13)

project(RevoMii
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =============================================================================
# Options
# =============================================================================

option(REVOMII_LINK_AURORA
    "Automatically link against Aurora"
    OFF
)

option(REVOMII_FETCH_AURORA
    "Automatically fetch Aurora"
    OFF
)

option(REVOMII_DOLPHIN
    "Use Dolphin headers instead of Revolution (required for Aurora)"
    OFF
)

option(REVOMII_BUILD_TESTS
    "Build tests"
    OFF
)

# =============================================================================
# Aurora
# =============================================================================

if(REVOMII_FETCH_AURORA)
    include(FetchContent)

    FetchContent_Declare(
        aurora
        GIT_REPOSITORY https://github.com/encounter/aurora.git
        GIT_TAG 3227d76c60e1e782ca576610bce61c9e7744d8be
    )

    FetchContent_MakeAvailable(aurora)
endif()

# =============================================================================
# Dependencies
# =============================================================================

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies.cmake)

# =============================================================================
# RevoInternal (shared utilities)
# =============================================================================

set(REVOMII_INTERNAL_SOURCES)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/internal_sources.cmake)

add_library(RevoInternal STATIC
    ${REVOMII_INTERNAL_SOURCES}
)

add_library(RevoMii::RevoInternal ALIAS RevoInternal)

target_include_directories(RevoInternal
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =============================================================================
# RVLFaceLib
# =============================================================================

set(REVOMII_RVLFACE_SOURCES)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/lib_sources.cmake)

add_library(RVLFaceLib STATIC
    ${REVOMII_RVLFACE_SOURCES}
)

add_library(RevoMii::RVLFaceLib ALIAS RVLFaceLib)

target_include_directories(RVLFaceLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(RVLFaceLib
    PRIVATE
        RevoInternal
)

if(REVOMII_FETCH_AURORA)
    target_include_directories(RVLFaceLib
        PUBLIC
            ${aurora_SOURCE_DIR}/include
    )
    target_link_libraries(RVLFaceLib
        PUBLIC
            aurora::gx
            aurora::mtx
            aurora::vi
    )
    target_compile_definitions(RVLFaceLib PUBLIC
        DOLPHIN_INCLUDES=1
        TARGET_PC=1
    )
elseif(REVOMII_LINK_AURORA)
    target_link_libraries(RVLFaceLib
        PUBLIC
            aurora
            aurora::gx
    )
    target_compile_definitions(RVLFaceLib PRIVATE
        DOLPHIN_INCLUDES=1
        TARGET_PC=1
    )
elseif(REVOMII_DOLPHIN)
    target_compile_definitions(RVLFaceLib PRIVATE
        DOLPHIN_INCLUDES=1
        TARGET_PC=1
    )
endif()

# =============================================================================
# RevoMii
# =============================================================================

set(REVOMII_SOURCES)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/revo_sources.cmake)

add_library(RevoMii STATIC
    ${REVOMII_SOURCES}
)

add_library(RevoMii::RevoMii ALIAS RevoMii)

target_include_directories(RevoMii
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(RevoMii
    PUBLIC
        RVLFaceLib
    PRIVATE
        RevoInternal
)

# =============================================================================
# Tests
# =============================================================================

if(REVOMII_BUILD_TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
    )
    FetchContent_MakeAvailable(Catch2)

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    include(CTest)
    include(Catch)

    add_subdirectory(tests)
endif()

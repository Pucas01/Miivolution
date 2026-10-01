cmake_minimum_required(VERSION 3.13)

project(Miivolution
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =============================================================================
# Options
# =============================================================================

option(MIIVOLUTION_LINK_AURORA
    "Automatically link against Aurora"
    OFF
)

option(MIIVOLUTION_FETCH_AURORA
    "Automatically fetch Aurora"
    OFF
)

option(MIIVOLUTION_DOLPHIN
    "Use Dolphin headers instead of Revolution (required for Aurora)"
    OFF
)

option(MIIVOLUTION_BUILD_TESTS
    "Build tests"
    OFF
)

# =============================================================================
# Aurora
# =============================================================================

if(MIIVOLUTION_FETCH_AURORA)
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

set(MIIVOLUTION_INTERNAL_SOURCES)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/internal_sources.cmake)

add_library(RevoInternal STATIC
    ${MIIVOLUTION_INTERNAL_SOURCES}
)

add_library(Miivolution::RevoInternal ALIAS RevoInternal)

target_include_directories(RevoInternal
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =============================================================================
# RVLFaceLib
# =============================================================================

set(RVLFACE_SOURCES)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/lib_sources.cmake)

add_library(RVLFaceLib STATIC
    ${RVLFACE_SOURCES}
)

add_library(Miivolution::RVLFaceLib ALIAS RVLFaceLib)

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

if(MIIVOLUTION_FETCH_AURORA)
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
elseif(MIIVOLUTION_LINK_AURORA)
    target_link_libraries(RVLFaceLib
        PUBLIC
            aurora
            aurora::gx
    )
    target_compile_definitions(RVLFaceLib PRIVATE
        DOLPHIN_INCLUDES=1
        TARGET_PC=1
    )
elseif(MIIVOLUTION_DOLPHIN)
    target_compile_definitions(RVLFaceLib PRIVATE
        DOLPHIN_INCLUDES=1
        TARGET_PC=1
    )
endif()

# =============================================================================
# MIIVOLUTION
# =============================================================================

set(MIIVOLUTION_SOURCES)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/revo_sources.cmake)

add_library(Miivolution STATIC
    ${MIIVOLUTION_SOURCES}
)

add_library(Miivolution::Miivolution ALIAS Miivolution)

target_include_directories(Miivolution
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Miivolution
    PUBLIC
        RVLFaceLib
    PRIVATE
        RevoInternal
)

# =============================================================================
# Tests
# =============================================================================

if(MIIVOLUTION_BUILD_TESTS)
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

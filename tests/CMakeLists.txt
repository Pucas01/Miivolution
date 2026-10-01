add_executable(miivolution_tests
    test_main.cpp
    test_logging.cpp
    test_endian.cpp
    test_rfl_system.cpp
    test_rfl_model.cpp
    test_rfl_middledb.cpp
    test_rfl_nandloader.cpp
    test_rfl_resource.cpp
    test_rfl_shape.cpp
)

target_link_libraries(miivolution_tests
    PRIVATE
        Miivolution
        RevoInternal
        RVLFaceLib
        Catch2::Catch2WithMain
)

target_include_directories(miivolution_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

catch_discover_tests(miivolution_tests)

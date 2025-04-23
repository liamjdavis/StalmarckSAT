find_package(GTest REQUIRED)

# Enable the CTest testing infrastructure
enable_testing()

# Unit tests
add_executable(unit_tests
    unit/test_formula.cpp
    unit/test_solver.cpp
    unit/test_parser.cpp
)

target_link_libraries(unit_tests
    PRIVATE
    stalmarck
    GTest::GTest
    GTest::Main
)

# Add the executable as a CTest test case
add_test(NAME unit_tests COMMAND unit_tests)

# Integration tests
add_executable(integration_tests integration/test_integration.cpp)
target_link_libraries(integration_tests
    PRIVATE
        stalmarck
        GTest::gtest
        GTest::gtest_main
)

# Add integration test with working directory set to project root
add_test(NAME integration_tests COMMAND integration_tests)
set_tests_properties(integration_tests 
    PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Fuzzing tests
add_executable(fuzzing_tests fuzzing/solver_fuzzer.cpp)
target_link_libraries(fuzzing_tests
    PRIVATE
        stalmarck
        GTest::gtest
        GTest::gtest_main
)

# Add fuzzing test (not part of automatic test suite by default)
add_test(NAME fuzzing_tests COMMAND fuzzing_tests)
set_tests_properties(fuzzing_tests 
    PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    LABELS "fuzzing"
)
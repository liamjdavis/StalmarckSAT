find_package(GTest REQUIRED)

# Enable the CTest testing infrastructure
enable_testing()

# Unit tests
add_executable(unit_tests
    unit/test_formula.cpp
)

target_link_libraries(unit_tests
    PRIVATE
    stalmarck   # Make sure this target is defined before this point or globally
    GTest::GTest
    GTest::Main # Links against the main() function provided by GTest
)

# Add the executable as a CTest test case
add_test(NAME unit_tests COMMAND unit_tests)
# Unit tests
add_executable(unit_tests
    unit/test_solver.cpp
)

target_link_libraries(unit_tests
    PRIVATE
    stalmarck
    GTest::GTest
    GTest::Main
)

add_test(NAME unit_tests COMMAND unit_tests)

# Integration tests
add_executable(integration_tests
    integration/test_stalmarck.cpp
)

target_link_libraries(integration_tests
    PRIVATE
    stalmarck
    GTest::GTest
    GTest::Main
)

add_test(NAME integration_tests COMMAND integration_tests) 
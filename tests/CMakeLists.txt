add_executable(tests
        main.cpp
        TestWorld.cpp
        TestCPPMethod.cpp
        TestIR.cpp
        TestDataflowFacts.cpp
        TestReachingDefinition.cpp
        )

target_link_libraries(tests
        libanalyzer
        )

target_compile_definitions(tests
        PUBLIC
        DOCTEST_CONFIG_USE_STD_HEADERS
        )

add_test(NAME doctest COMMAND tests)

set_tests_properties(doctest
        PROPERTIES
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )


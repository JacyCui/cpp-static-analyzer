add_executable(tests
        main.cpp
        TestWorld.cpp
        )

target_link_libraries(tests
        libanalyzer
        )

add_test(NAME doctest COMMAND tests)

set_tests_properties(doctest
        PROPERTIES
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )


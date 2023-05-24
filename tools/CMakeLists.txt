add_executable(reaching-definition-analyzer reaching-definition-analyzer.cpp)

target_link_libraries(reaching-definition-analyzer
        libanalyzer
        )

add_executable(live-variable-analyzer live-variable-analyzer.cpp)

target_link_libraries(live-variable-analyzer
        libanalyzer
        )


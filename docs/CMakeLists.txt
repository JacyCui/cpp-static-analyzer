find_package(Doxygen
        REQUIRED dot
        OPTIONAL_COMPONENTS mscgen dia)

set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/api-doc)

doxygen_add_docs(libanalyzer-api-doc
        ${PROJECT_SOURCE_DIR}/include
        COMMENT "Generate html documentation of libanalyzer."
        )


find_package(Doxygen REQUIRED)
set(DOXYGEN_EXCLUDE_PATTERNS "${CMAKE_SOURCE_DIR}/ext/*")
set(DOXYGEN_SHORT_NAMES YES)
set(DOXYGEN_GENERATE_XML YES)

doxygen_add_docs(cpp-pointer-doxygen
  ${CMAKE_SOURCE_DIR}/include
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT Building doxygen documentation...
)

add_custom_target(cpp-pointer-sphinx-doc
  COMMAND
    sphinx-build -b html
    -Dbreathe_projects.cpp-pointer="${CMAKE_CURRENT_BINARY_DIR}/xml"
    -c ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/sphinx
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating documentation with Sphinx..."
)
add_dependencies(cpp-pointer-sphinx-doc cpp-pointer-doxygen)